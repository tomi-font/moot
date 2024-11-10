#include <moot/World.hh>
#include <moot/Entity/Entity.hh>
#include <moot/System/Types.hh>
#include <moot/utility/variant/indexToCompileTime.hh>
#include <SFML/Window/Event.hpp>

World::World(Window* window) :
	m_systems(std::tuple_size_v<Systems>),
	m_namedEntities({ .required = {CId<CName>} }),
	m_window(window),
	m_running(true)
{
	m_systems[SId<SInput>] = std::make_unique<SInput>();
	m_systems[SId<SPhysics>] = std::make_unique<SPhysics>();
	m_systems[SId<SRender>] = std::make_unique<SRender>();

	for (const auto& system: m_systems)
	{
		system->setWindow(window);

		system->setEventManager(&m_eventManager);
		system->listenToEvents();

		system->setProperties(&m_properties);
		system->initializeProperties();
	}

	setEventManager(&m_eventManager);
	listen(Event::GameClose);

	initializeScriptContext(this);
}

void World::triggered(const Event& event)
{
	assert(event.type == Event::GameClose);
	m_running = false;
}

void World::updateEntities()
{
	std::vector<std::tuple<ComponentComposition, Prototype>> entitiesToChange;
	entitiesToChange.reserve(m_entitiesToChange.size());

	// First remove the entities. That invalidates references to other entities.
	for (const EntityContext& entity : m_entitiesToRemove)
	{
		// assert(entity.comp() == entity.m_arch->comp());

		const auto findIt = m_entitiesToChange.find(entity);
		if (findIt != m_entitiesToChange.end())
		{
			// assert(entity.comp() == findIt->first.comp());

			for (const auto& system: m_systems)
				system->entityChangedRemovedCallback(entity, findIt->second.comp());
			
			entitiesToChange.emplace_back(entity.m_arch->comp(), std::move(findIt->second));
			m_entitiesToChange.erase(findIt);
		}
		else
		{
			for (const auto& system: m_systems)
				system->entityRemovedCallback(entity);
		}

		entity.m_arch->remove(entity.m_idx);
	}
	assert(m_entitiesToChange.empty());
	m_entitiesToRemove.clear();

	// References to entities cannot be stored between frames.
	// This is the moment there must not be any extra reference left because they are invalidated by removal and modification of entities.
	assert(EntityContext::instanceCount() == 0);

	for (const auto& [oldComp, proto] : entitiesToChange)
	{
		const Entity entity = getArchetype(proto.comp())->instantiate(proto);

		for (const auto& system: m_systems)
			system->entityChangedAddedCallback(entity, oldComp);
	}
	entitiesToChange.clear();

	std::unordered_set<Entity> instantiatedEntities;

	for (const Prototype& proto : m_entitiesToInstantiate)
	{
		const Entity entity = getArchetype(proto.comp())->instantiate(proto);

		for (const auto& system : m_systems)
			system->entityAddedCallback(entity);

		instantiatedEntities.insert(entity);
	}
	m_entitiesToInstantiate.clear();

	for (const Entity& entity : instantiatedEntities)
	{
		if (CCallback* cCallback = entity.getOrNull<CCallback*>())
		{
			if (const auto& onSpawn = cCallback->extract(CCallback::OnSpawn))
			{
				onSpawn(entity);
			}
		}
	}
}

void World::update()
{
	m_properties.set(Property::ElapsedTime, m_clock.restart().asSeconds());

	// First remove and add existing entities' components.
	// References to them get invalidated when removing entities.
	updateEntitiesComponents();

	// Then remove and instantiate the entities that are waiting for that.
	updateEntities();

	for (const auto& system : m_systems)
	{
		system->performUpdate();
	}
	
	updateScriptContext();
}

Archetype* World::findArchetype(ComponentComposition comp)
{
	for (Archetype& arch : m_archs)
	{
		if (arch.comp() == comp)
			return &arch;
	}
	return nullptr;
}

Archetype* World::getArchetype(ComponentComposition comp)
{
	Archetype* arch = findArchetype(comp);
	if (arch)
		return arch;

	// If we didn't find an archetype matching the composition,
	// it means it doesn't exist so we have to create it.
	arch = &m_archs.emplace_back(comp, this);

	// Then we must iterate the systems to see whether they're interested.
	for (const auto& system : m_systems)
	{
		system->match(arch);
	}
	m_namedEntities.match(arch);

	return arch;
}

Entity World::upToDateCompo(Entity entity) const
{
	const auto addedComponentsIt = m_componentsToAdd.find(entity);
	if (addedComponentsIt != m_componentsToAdd.end())
		for (const auto& [cid, _] : addedComponentsIt->second)
			entity.m_comp += cid;

	const auto removedComponentsIt = m_componentsToRemove.find(entity);
	if (removedComponentsIt != m_componentsToRemove.end())
		for (const ComponentId cid : removedComponentsIt->second)
			entity.m_comp -= cid;
	
	return entity;
}

Entity World::findEntity(EntityId eId)
{
	// TODO: entity ID cache
	for (Archetype& arch : m_archs)
	{
		const auto& entityComponents = arch.getAll<CEntity>();
		for (const CEntity& cEntity : entityComponents)
		{
			if (cEntity.id() == eId)
			{
				const unsigned index = static_cast<unsigned>(&cEntity - entityComponents.data());
				return upToDateCompo(EntityContext(&arch, index));
			}
		}
	}
	return {};
}

std::optional<Entity> World::findEntity(std::string_view name)
{
	for (Entity entity : m_namedEntities)
	{
		if (entity.get<CName>() == name)
			return upToDateCompo(entity);
	}
	return {};
}

static void checkEntity(const Prototype& entity)
{
	if (!entity.has<CPosition>())
		assert(entity.hasNoneOf(CId<CCollisionBox> + CId<CConvexPolygon> + CId<CView> + CId<CMove> + CId<CRigidbody>));

	if (entity.has<CPointable>())
		assert(entity.has<CConvexPolygon>());
}

static void checkChangedEntity(const Prototype& entity)
{
	assert(entity.has<CEntity>());
	checkEntity(entity);
}

static void checkSpawnedEntity(const Prototype& entity)
{
	assert(!entity.has<CChildren>());
	assert(!entity.has<CParent>());
	checkEntity(entity);
}

EntityId World::spawn(Prototype* entity, std::optional<sf::Vector2f> pos)
{
	if (pos)
		entity->add<CPosition>(*pos);

	const EntityId eId = entity->add<CEntity>()->id();

	checkSpawnedEntity(*entity);

	return eId;
}

void World::spawnChildOf(Entity* parent, const Prototype& childProto, std::optional<sf::Vector2f> pos)
{
	Prototype& child = m_entitiesToInstantiate.emplace_back(childProto);
	const EntityId childEId = spawn(&child, pos);
	child.add<CParent>(parent->getId());

	CChildren* const cChildren = parent->has<CChildren>() ? parent->get<CChildren*>() : parent->add<CChildren>();
	cChildren->add(childEId);
}

void World::spawnChildOf(EntityId parentEid, const Prototype& childProto, std::optional<sf::Vector2f> pos)
{
	assert(findEntity(parentEid).isEmpty());

	Prototype& child = m_entitiesToInstantiate.emplace_back(childProto);
	const EntityId childEId = spawn(&child, pos);
	child.add<CParent>(parentEid);

	for (Prototype& proto : m_entitiesToInstantiate)
	{
		if (proto.get<CEntity>().id() == parentEid)
		{
			CChildren* const cChildren = proto.has<CChildren>() ? proto.get<CChildren*>() : proto.add<CChildren>();
			cChildren->add(childEId);
			return;
		}
	}
	assert(false);
}

void World::remove(EntityContext entity)
{
	m_componentsToRemove.erase(entity);
	m_componentsToAdd.erase(entity);
	m_entitiesToRemove.insert(entity);
}

ComponentVariant* World::addComponentTo(const EntityContext& ec, ComponentVariant&& component)
{	
	const ComponentId cid = CVId(component);

	assert(!ec.isEmpty() && !m_entitiesToRemove.contains(ec));

	if (m_componentsToRemove.contains(ec))
	{
		// A component remove() + add() + get() would return the one that was originally removed.
		assert(!m_componentsToRemove.at(ec).contains(cid));
	}
	
	const auto& insertionPair = m_componentsToAdd[ec].emplace(cid, std::move(component));
	assert(insertionPair.second);
	return &insertionPair.first->second;
}

void World::removeComponentFrom(const EntityContext& ec, ComponentId cid)
{
	assert(cid != CId<CEntity>);
	assert(!ec.isEmpty() && !m_entitiesToRemove.contains(ec));
	
	if (m_componentsToAdd.contains(ec))
	{
		if (m_componentsToAdd.at(ec).erase(cid))
			return;
	}
	const bool removed = m_componentsToRemove[ec].insert(cid).second;
	assert(removed);
}

ComponentVariant* World::getStagedComponentOf(const EntityContext& entity, ComponentId cid)
{
	return &m_componentsToAdd.at(entity).at(cid);
}

void World::updateEntitiesComponents()
{
	std::unordered_set<EntityContext> entities;
	for (const auto& [entity, _] : m_componentsToAdd)
		entities.insert(entity);
	for (const auto& [entity, _] : m_componentsToRemove)
		entities.insert(entity);

	for (EntityContext entity : entities)
	{
		Archetype* const arch = entity.m_arch;

		// Schedule the entity to be removed and its changed version to be added.
		// All the entities must be removed in a controlled order because the removal invalidates references to other entities.
		m_entitiesToRemove.insert(entity);
		Prototype& proto = m_entitiesToChange[entity];

		const auto& componentsToRemove = m_componentsToRemove[entity];
	
		// Start with the entity's original components minus those that are to be removed.
		for (ComponentId cid : arch->comp())
		{
			if (!componentsToRemove.contains(cid))
			{
				variantIndexToCompileTime<ComponentVariant>(cid,
					[&](auto I)
					{
						using C = std::tuple_element_t<I, Components>;
						proto.add<C>(*arch->get<C>(entity.m_idx));
					});
			}
		}
		// And to them add the components to be added.
		for (auto& [cid, component] : m_componentsToAdd[entity])
			proto.add(std::move(component));

		checkChangedEntity(proto);
	}
	m_componentsToAdd.clear();
	m_componentsToRemove.clear();
}
