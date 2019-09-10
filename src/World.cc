#include <World.hh>

#include <SFML/Window/Event.hpp>

World::World(sf::RenderWindow& w) : m_window(w)
{
	m_systems[System::Render] = &m_srender;
}

Archetype&	World::getArchetype(t_Comp comp)
{
	for (Archetype& a : m_archs)
	{
		if (a.getComp() == comp)
			return a;
	}

// if we didn't find an archetype matching the composition,
// it means it doesn't exist so we have to create it
// we use emplace_back() so that no useless copy is made
	m_archs.emplace_back(comp);

	Archetype&	arch = m_archs.back();

// then we must iterate through systems to see if they're interested
	for (System* s : m_systems)
	{
	// each system may have several groups of interest
		for (ComponentGroup& group : s->getGroups())
		{
			if ((comp & group.inc) == group.inc && !(comp & group.exc))
			{
				group.archs.push_back(&arch);
			}
		}
	}

	return arch;
}

bool	World::turn()
{
	m_srender.render(m_window);

	for (sf::Event e; m_window.pollEvent(e);)
	{
		if (e.type == sf::Event::Closed)
			return false;
	}

	return true;
}
