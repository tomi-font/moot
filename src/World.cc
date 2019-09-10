#include <World.hh>

World::World()
{
// we know there is going to be many archetypes, better reserve some space
	m_archs.reserve(10);

	m_systems[System::Render] = &m_srender;
}
#include <iostream>
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
				std::cout << "oui" << std::endl;
				group.archs.push_back(&arch);
			}
		}
	}

	return arch;
}

bool	World::turn()
{
	m_srender.update();

	return true;
}
