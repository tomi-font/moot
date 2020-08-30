#include <World.hh>

#include <SFML/Window/Event.hpp>

World::World(sf::RenderWindow& w) : m_window(w)
{
	m_systems[System::Input] = &m_sinput;
	m_systems[System::Physics] = &m_sphysics;
	m_systems[System::Render] = &m_srender;

	m_factory.create("player", this);
	m_factory.create("platform", this);
}

Archetype*	World::getArchetype(CsComp comp)
{
	for (Archetype& a : m_archs)
	{
		if (a.getComp() == comp)
			return &a;
	}

// if we didn't find an archetype matching the composition,
// it means it doesn't exist so we have to create it
// we use emplace_back() so that no useless copy is made
	Archetype*	arch = &m_archs.emplace_back(comp);

// then we must iterate through systems to see if they're interested
	for (System* system : m_systems)
	{
	// each system may have several groups of interest
		for (ComponentGroup& group : system->getGroups())
		{
			if ((comp & group.inc) == group.inc && !(comp & group.exc))
			{
				group.archs.push_back(arch);
			}
		}
	}
	return arch;
}

bool	World::turn()
{
	float	elapsedTime = m_clock.restart().asSeconds();
	bool	turning;

	turning = m_sinput.readInput(m_window);
	m_sphysics.enforce(elapsedTime);
	m_srender.render(m_window);

	return turning;
}
