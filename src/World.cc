#include <World.hh>
#include <System/Types.hh>
#include <SFML/Window/Event.hpp>

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
		system->setWindow(window);
	}

	// Restart the clock to not count the setup time.
	m_clock.restart();
}

void World::update()
{
	const float elapsedTime = m_clock.restart().asSeconds();

	for (const auto& system : m_systems)
	{
		if (!m_running)
			break;
		system->update(elapsedTime);
	}
}

void World::instantiate(const Template& temp)
{
	getArchetype(temp.comp())->instantiate(temp);
}

Archetype* World::getArchetype(ComponentComposition comp)
{
	for (Archetype& arch : m_archs)
	{
		if (arch.comp() == comp)
			return &arch;
	}

	// If we didn't find an archetype matching the composition,
	// it means it doesn't exist so we have to create it.
	Archetype* arch = &m_archs.emplace_back(comp);

	// Then we must iterate the systems to see whether they're interested.
	for (const auto& system : m_systems)
	{
		system->match(arch);
	}
	return arch;
}

EntityHandle World::getEntity(const std::string& name)
{
	for (Archetype& arch : m_archs)
	{
		if (arch.has<CName>())
		{
			const std::span cNames = arch.getAll<CName>();
			const auto cNameIt = std::find(cNames.begin(), cNames.end(), name);
			
			if (cNameIt != cNames.end())
				return { &arch, static_cast<unsigned int>(cNameIt - cNames.begin()) };
		}
	}
	return {};
}
