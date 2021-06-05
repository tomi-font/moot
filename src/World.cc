#include <World.hh>
#include <System/Types.hh>
#include <SFML/Window/Event.hpp>

World::World() : EventListener(m_eventManager), m_running(true), m_systems(std::tuple_size_v<Systems>)
{
	m_systems[SId<SInput>] = std::make_unique<SInput>(m_eventManager);
	m_systems[SId<SPhysics>] = std::make_unique<SPhysics>();
	m_systems[SId<SRender>] = std::make_unique<SRender>();

	listen(Event::PlayerQuit);

// Restart the clock to not count the setup time.
	m_clock.restart();
}

Archetype*	World::getArchetype(ComponentComposition comp)
{
	for (Archetype& a : m_archs)
	{
		if (a.comp() == comp)
			return &a;
	}

// if we didn't find an archetype matching the composition,
// it means it doesn't exist so we have to create it
// we use emplace_back() so that no useless copy is made
	Archetype*	arch = &m_archs.emplace_back(comp);

// then we must iterate through systems to see if they're interested
	for (const auto& system : m_systems)
	{
		system->match(arch);
	}
	return arch;
}

void	World::update(sf::RenderWindow& window)
{
	float	elapsedTime = m_clock.restart().asSeconds();

	for (const auto& system : m_systems)
	{
		if (!m_running)
			break;
		system->update(window, elapsedTime);
	}
}

void	World::triggered(const Event&)
{
	m_running = false;
}
