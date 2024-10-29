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

	m_parsingContext.initialize(this);
}

void World::triggered(const Event& event)
{
	assert(event.type == Event::GameClose);
	m_running = false;
}

void World::processScript(const std::string& path)
{
	m_parsingContext.process(path);
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
	const float elapsedTime = m_clock.restart().asSeconds();

	// First remove and add existing entities' components.
	// References to them get invalidated when removing entities.
	updateEntitiesComponents();

	// Then remove and instantiate the entities that are waiting for that.
	updateEntities();

	for (const auto& system : m_systems)
	{
		system->gameLoopUpdate(elapsedTime);
	}
	
	m_parsingContext.update();
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

std::optional<Entity> World::findEntity(std::string_view name)
{
	for (Entity entity : m_namedEntities)
	{
		if (entity.get<CName>() == name)
			return entity;
	}
	return {};
}

static void processEntityToBeAdded(const Prototype& entity)
{
	if (!entity.has<CPosition>())
		assert(entity.hasNoneOf(CId<CCollisionBox> + CId<CConvexPolygon> + CId<CView> + CId<CMove> + CId<CRigidbody>));

	if (entity.has<CPointable>())
		assert(entity.has<CConvexPolygon>());
}

static void processChangedEntity(const Prototype& entity)
{
	assert(entity.has<CEntity>());
	processEntityToBeAdded(entity);
}

static void processInstantiatedEntity(Prototype* entity)
{
	entity->add<CEntity>();
	processEntityToBeAdded(*entity);
}

void World::instantiate(const Prototype& proto)
{
	Prototype& entity = m_entitiesToInstantiate.emplace_back(proto);
	processInstantiatedEntity(&entity);
}

void World::instantiate(const Prototype& proto, const sf::Vector2f& pos)
{
	Prototype& entity = m_entitiesToInstantiate.emplace_back(proto);
	entity.add<CPosition>(pos);
	processInstantiatedEntity(&entity);
}

void World::instantiate(const std::string& protoName)
{
	instantiate(m_prototypeStore.getPrototype(protoName));
}

void World::instantiate(const std::string& protoName, const sf::Vector2f& pos)
{
	instantiate(m_prototypeStore.getPrototype(protoName), pos);
}

void World::remove(EntityContext entity)
{
	m_componentsToRemove.erase(entity);
	m_componentsToAdd.erase(entity);
	m_entitiesToRemove.insert(entity);
}

void World::addComponentTo(Entity* entity, ComponentVariant&& component)
{	
	const ComponentId cid = CVId(component);

	EntityContext ec = *entity;
	assert(!m_entitiesToRemove.contains(ec));

	if (m_componentsToRemove.contains(ec))
	{
		// A component remove() + add() + get() would return the one that was originally removed.
		assert(!m_componentsToRemove.at(ec).contains(cid));
	}
	
	const bool added = m_componentsToAdd[ec].emplace(cid, std::move(component)).second;
	assert(added);
}

void World::removeComponentFrom(Entity* entity, ComponentId cid)
{
	assert(cid != CId<CEntity>);

	EntityContext ec = *entity;
	assert(!m_entitiesToRemove.contains(ec));
	
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

		processChangedEntity(proto);
	}
	m_componentsToAdd.clear();
	m_componentsToRemove.clear();
}
