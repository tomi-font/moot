#include <moot/Game.hh>
#include <moot/Component/Id.hh>
#include <moot/Entity/Entity.hh>
#include <moot/Event/Engine.hh>
#include <moot/System/SInput.hh>
#include <moot/System/SPhysics.hh>
#include <moot/System/SRender.hh>
#include <moot/util/variant/indexToCompileTime.hh>
#include <SFML/Window/Event.hpp>

unsigned ComponentIdRegistry::s_m_nextId = 0;

Game::Game() :
	m_namedEntities({ .required = {CId<CName>} }),
	m_running(true)
{
	assert(ComponentIdRegistry::idCount() <= 8 * sizeof(ComponentComposition::Bits));

	addSystem<SInput>({});
	addSystem<SPhysics>(SystemSchedule::after<SInput>());
	addSystem<SRender>(SystemSchedule::after<SPhysics>());

	setEventManager(&m_eventManager);
	listenTo(EngineEvent::GameClose);

	initializeScriptContext(this);

	sf::VideoMode halfScreen = sf::VideoMode::getDesktopMode();
	halfScreen.width /= 2;
	halfScreen.height /= 2;
	m_window.create(halfScreen, {});
	m_window.setPosition(sf::Vector2i(halfScreen.width / 2, halfScreen.height / 2));
	m_window.setFramerateLimit(60);
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(false);
}

void Game::onSystemAdded(System* system)
{
	assert(m_entityIdMap.empty());

	system->setWindow(&m_window);

	system->setEventManager(&m_eventManager);
	system->listenToEvents();

	system->setProperties(&m_properties);
	system->initializeProperties();
}

void Game::onEvent(const Event& event)
{
	assert(event.id == EngineEvent::GameClose);
	m_running = false;
}

void Game::updateEntities()
{
	std::unordered_map<Archetype*, unsigned> alteredArchs;
	std::vector<std::tuple<ComponentComposition, Prototype>> entitiesToChange;
	entitiesToChange.reserve(m_entitiesToChange.size());

	// First remove the entities. That invalidates references to other entities.
	for (const EntityPointer& entity : m_entitiesToRemove)
	{
		const auto findIt = m_entitiesToChange.find(entity);
		if (findIt != m_entitiesToChange.end())
		{
			for (const auto& system: m_systems)
				system->onChangedEntityRemoved(entity, findIt->second.comp());
			
			entitiesToChange.emplace_back(entity.arch()->comp(), std::move(findIt->second));
			m_entitiesToChange.erase(findIt);
		}
		else
		{
			for (const auto& system: m_systems)
				system->onEntityRemoved(entity);

			const bool erased = m_entityIdMap.erase(Entity(entity).getId());
			assert(erased);
		}

		entity.arch()->remove(entity.index());
		alteredArchs[entity.arch()] = entity.index();
	}
	assert(m_entitiesToChange.empty());
	m_entitiesToRemove.clear();

	for (auto [arch, idx] : alteredArchs)
	{
		const auto& entityComponents = arch->getAll<CEntity>();
		while (idx != entityComponents.size())
		{
			const EntityId eId = entityComponents[idx].id();
			m_entityIdMap.at(eId) = {arch, idx};
			++idx;
		}
	}
	alteredArchs.clear();
#ifndef NDEBUG
	unsigned instantiatedEntityCount = 0;
	for (const Archetype& arch : m_archs)
		instantiatedEntityCount += arch.entityCount();
	assert(instantiatedEntityCount + entitiesToChange.size() == m_entityIdMap.size());
#endif

	// References to entities cannot be stored between frames.
	// This is the moment there must not be any extra reference left because they are invalidated by removal and modification of entities.
	assert(EntityPointer::instanceCount() == m_entityIdMap.size());

	for (const auto& [oldComp, proto] : entitiesToChange)
	{
		const Entity entity = getArchetype(proto.comp())->instantiate(proto);

		const auto insPair = m_entityIdMap.insert_or_assign(entity.getId(), entity);
		assert(!insPair.second);

		for (const auto& system: m_systems)
			system->onChangedEntityAdded(entity, oldComp);
	}
	entitiesToChange.clear();

	std::unordered_set<Entity> instantiatedEntities;

	for (const Prototype& proto : m_entitiesToInstantiate)
	{
		const Entity entity = getArchetype(proto.comp())->instantiate(proto);

		const auto insPair = m_entityIdMap.emplace(entity.getId(), entity);
		assert(insPair.second);

		for (const auto& system : m_systems)
			system->onEntityAdded(entity);

		instantiatedEntities.insert(entity);
	}
	m_entitiesToInstantiate.clear();

	for (const Entity& entity : instantiatedEntities)
	{
		if (CCallback* cCallback = entity.find<CCallback*>())
		{
			if (const auto& onSpawn = cCallback->extract(CCallback::OnSpawn))
			{
				onSpawn(entity);
			}
		}
	}
}

void Game::play()
{
	m_clock.restart();

	while (m_running)
	{
		m_properties.set(Property::ElapsedTime, m_clock.restart().asSeconds());

		// First remove and add existing entities' components.
		// References to them get invalidated when removing entities.
		updateEntitiesComponents();

		// Then remove and instantiate the entities that are waiting for that.
		updateEntities();

		updateSystems();

		updateScriptContext();
	}
}

Archetype* Game::findArchetype(ComponentComposition comp)
{
	for (Archetype& arch : m_archs)
	{
		if (arch.comp() == comp)
			return &arch;
	}
	return nullptr;
}

Archetype* Game::getArchetype(ComponentComposition comp)
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

Entity Game::upToDateCompo(Entity entity) const
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

Entity Game::getEntity(EntityId eId) const
{
	return upToDateCompo(m_entityIdMap.at(eId));
}

std::optional<Entity> Game::findEntity(std::string_view name) const
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

EntityId Game::spawn(Prototype* entity, std::optional<sf::Vector2f> pos)
{
	if (pos)
		entity->add<CPosition>(*pos);

	const EntityId eId = entity->add<CEntity>()->id();

	checkSpawnedEntity(*entity);

	return eId;
}

void Game::spawnChildOf(Entity* parent, const Prototype& childProto, std::optional<sf::Vector2f> pos)
{
	Prototype& child = m_entitiesToInstantiate.emplace_back(childProto);
	const EntityId childEId = spawn(&child, pos);
	child.add<CParent>(parent->getId());

	CChildren* const cChildren = parent->has<CChildren>() ? parent->get<CChildren*>() : parent->add<CChildren>();
	cChildren->add(childEId);
}

void Game::spawnChildOf(EntityId parentEid, const Prototype& childProto, std::optional<sf::Vector2f> pos)
{
	assert(!m_entityIdMap.contains(parentEid));

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

void Game::recursivelyRemove(Entity entity)
{
	m_componentsToRemove.erase(entity);
	m_componentsToAdd.erase(entity);
	m_entitiesToRemove.insert(entity);

	if (const CChildren* cChildren = entity.find<CChildren*>())
		for (EntityId childEId : cChildren->eIds())
			recursivelyRemove(m_entityIdMap.at(childEId));
}

void Game::remove(const Entity& entity)
{	
	if (const CParent* cParent = entity.find<CParent*>())
		Entity(m_entityIdMap.at(*cParent)).get<CChildren*>()->remove(entity.getId());

	recursivelyRemove(entity);
}

ComponentVariant* Game::addComponentTo(Entity* entity, ComponentVariant&& component)
{
	const ComponentId cid = CVId(component);
	const EntityPointer ePtr = *entity;
	assert(ePtr.isValid() && !m_entitiesToRemove.contains(ePtr));
	entity->m_comp += cid;

	if (m_componentsToRemove.contains(ePtr))
	{
		// A component remove() + add() + get() would return the one that was originally removed.
		assert(!m_componentsToRemove.at(ePtr).contains(cid));
	}
	
	const auto& insertionPair = m_componentsToAdd[ePtr].emplace(cid, std::move(component));
	assert(insertionPair.second);
	return &insertionPair.first->second;
}

void Game::removeComponentFrom(Entity* entity, ComponentId cid)
{
	const EntityPointer ePtr = *entity;
	assert(cid != CId<CEntity>);
	assert(ePtr.isValid() && !m_entitiesToRemove.contains(ePtr));
	entity->m_comp -= cid;
	
	if (m_componentsToAdd.contains(ePtr))
	{
		if (m_componentsToAdd.at(ePtr).erase(cid))
			return;
	}
	const bool removed = m_componentsToRemove[ePtr].insert(cid).second;
	assert(removed);
}

ComponentVariant* Game::getStagedComponentOf(const EntityPointer& entity, ComponentId cid)
{
	return &m_componentsToAdd.at(entity).at(cid);
}

void Game::updateEntitiesComponents()
{
	std::unordered_set<EntityPointer> entities;
	for (const auto& [entity, _] : m_componentsToAdd)
		entities.insert(entity);
	for (const auto& [entity, _] : m_componentsToRemove)
		entities.insert(entity);

	for (EntityPointer entity : entities)
	{
		Archetype* const arch = entity.arch();

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
						proto.add<C>(*arch->get<C>(entity.index()));
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
