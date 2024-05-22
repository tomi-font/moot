#include <World.hh>
#include <Entity/Entity.hh>
#include <System/Types.hh>
#include <utility/variant/indexToCompileTime.hh>
#include <SFML/Window/Event.hpp>

World::World(Window* window) :
	m_systems(std::tuple_size_v<Systems>),
	m_namedEntities(CId<CName>),
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

		system->setPropertyManager(&m_propertyManager);
		system->initializeProperties();
	}
}

void World::updateEntities()
{
	for (const EntityContext& entity : m_entitiesToRemove)
	{
		entity.m_arch->remove(entity.m_idx);
	}
	m_entitiesToRemove.clear();

	// References to entities cannot be stored between frames.
	// This is the moment there must not be any reference left because they are invalidated by removal and instantiation of entities.
	assert(EntityContext::instanceCount() == 0);

	std::unordered_set<Entity> instantiatedEntities;

	for (const Template& temp : m_entitiesToInstantiate)
	{
		const ComponentComposition comp = temp.comp();
		const Entity entity = getArchetype(comp)->instantiate(temp);

		for (const auto& system : m_systems)
			system->processInstantiatedEntity(entity);

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
		system->update(elapsedTime);
	}
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

static void prepareForInstantiation(const Template& entity)
{
	if (!entity.has<CPosition>())
		assert(entity.comp().hasNoneOf(CId<CCollisionBox> + CId<CRender> + CId<CView> + CId<CMove> + CId<CRigidbody>));
}

void World::instantiate(const Template& temp)
{
	Template& entity = m_entitiesToInstantiate.emplace_back(temp);
	prepareForInstantiation(entity);
}

void World::instantiate(const Template& temp, const sf::Vector2f& pos)
{
	Template& entity = m_entitiesToInstantiate.emplace_back(temp);
	entity.add<CPosition>(pos);
	prepareForInstantiation(entity);
}

void World::remove(EntityContext entity)
{
	m_componentsToRemove.erase(entity);
	m_componentsToAdd.erase(entity);
	m_entitiesToRemove.insert(entity);
}

void World::addComponentTo(EntityContext* entity, ComponentVariant&& component)
{	
	const ComponentId cid = CVId(component);
	entity->m_comp += cid;

	EntityContext ec = *entity;
	// Clear the composition to make sure it doesn't get used as it would fall out of date.
	ec.m_comp = {};
	assert(!m_entitiesToRemove.contains(ec));

	if (m_componentsToRemove.contains(ec))
	{
		// A component remove() + add() + get() would return the one that was originally removed.
		assert(!m_componentsToRemove.at(ec).contains(cid));
	}
	
	const bool added = m_componentsToAdd[ec].emplace(cid, std::move(component)).second;
	assert(added);
}

void World::removeComponentFrom(EntityContext* entity, ComponentId cid)
{
	assert(entity->has(cid));
	entity->m_comp -= cid;

	EntityContext ec = *entity;
	// Clear the composition to make sure it doesn't get used as it would fall out of date.
	ec.m_comp = {};
	assert(!m_entitiesToRemove.contains(ec));
	
	if (m_componentsToAdd.contains(ec))
	{
		const bool removed = m_componentsToAdd.at(ec).erase(cid);
		if (removed)
			return;
	}
	const bool removed = m_componentsToRemove[ec].insert(cid).second;
	assert(removed);
}

ComponentVariant* World::getStagedComponentOf(const EntityContext& entity, ComponentId cid)
{
	assert(entity.has(cid));
	assert(m_componentsToAdd.contains(entity));
	auto& components = m_componentsToAdd.at(entity);
	
	assert(components.contains(cid));
	return &components.at(cid);
}

void World::updateEntitiesComponents()
{
	std::unordered_set<EntityContext> entities;
	for (const auto& [entity, components] : m_componentsToAdd)
		entities.insert(entity);
	for (const auto& [entity, components] : m_componentsToRemove)
		entities.insert(entity);

	for (const EntityContext& entity : entities)
	{
		Archetype* origArch = entity.m_arch;
		const ComponentComposition origComp = origArch->comp();
	
		// TODO: Do this without going through a Template, by appending directly to the new Archetype?

		// Schedule the entity to be removed and its updated version to be added.
		// The removal shall only happen in updateEntities() because it invalidates the indices of other entities.
		// Note that this means that the entity will temporarily not exist, which is totally fine as long as nothing is expected from it during that time.
		m_entitiesToRemove.insert(entity);
		Template& temp = m_entitiesToInstantiate.emplace_back();

		const auto& componentsToRemove = m_componentsToRemove[entity];
	
		// Start with the entity's original components minus those that are to be removed.
		for (ComponentId cid : origComp)
		{
			if (!componentsToRemove.contains(cid))
			{
				variantIndexToCompileTime<ComponentVariant>(cid,
					[&](auto I)
					{
						using C = std::tuple_element_t<I, Components>;
						temp.add<C>(*origArch->get<C>(entity.m_idx));
					});
			}
		}
		// And to them add the components to be added.
		for (auto& [cid, component] : m_componentsToAdd[entity])
			temp.add(std::move(component));
		
		prepareForInstantiation(temp);
	}
	m_componentsToAdd.clear();
	m_componentsToRemove.clear();
}
