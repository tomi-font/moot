#include <World.hh>
#include <System/Types.hh>
#include <cstdint>
#include <SFML/Window/Event.hpp>

inline std::size_t std::hash<EntityContext>::operator()(const EntityContext& ec) const
{
	constexpr auto shift = 8 * (sizeof(std::size_t) - sizeof(ec.m_idx));
	static_assert(shift > 0);

	const std::size_t index = ec.m_idx;
	const std::uintptr_t archAddr = reinterpret_cast<std::uintptr_t>(ec.m_arch);

	return (index << shift) | archAddr;
}

World::World(sf::RenderWindow* window) :
	m_systems(std::tuple_size_v<Systems>),
	m_running(true)
{
	m_systems[SId<SInput>] = std::make_unique<SInput>();
	m_systems[SId<SPhysics>] = std::make_unique<SPhysics>();
	m_systems[SId<SRender>] = std::make_unique<SRender>();

	for (const auto& system: m_systems)
	{
		system->setEventManager(&m_eventManager);
		system->listenToEvents();
		system->initWindow(window);
	}

	// Restart the clock to not count the setup time.
	m_clock.restart();
}

void World::updateEntities()
{
	for (const EntityContext& entity : m_entitiesToRemove)
	{
		entity.m_arch->remove(entity);
	}
	m_entitiesToRemove.clear();

	for (const Template& temp : m_entitiesToInstantiate)
	{
		getArchetype(temp.comp())->instantiate(temp);
	}
	m_entitiesToInstantiate.clear();
}

void World::update()
{
	const float elapsedTime = m_clock.restart().asSeconds();

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
	return arch;
}

EntityContext World::findEntity(const std::string& name)
{
	for (Archetype& arch : m_archs)
	{
		if (arch.has<CName>())
		{
			const std::span cNames = arch.getAll<CName>();
			const auto cNameIt = std::find(cNames.begin(), cNames.end(), name);
			
			if (cNameIt != cNames.end())
				return { arch.comp(), &arch, static_cast<unsigned int>(cNameIt - cNames.begin()) };
		}
	}
	return {};
}

void World::remove(EntityContext&& entity)
{
	m_entitiesToRemove.insert(std::move(entity));
}

