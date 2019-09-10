#include <World.hh>

Archetype&	World::getArchetype(t_Comp c)
{
	for (Archetype& a : m_archs)
	{
		if (a.getComp() == c)
			return a;
	}

// if we didn't find an archetype matching the composition,
// it means it doesn't exist so we have to create it
// we use emplace_back() so that no useless copy is made
	m_archs.emplace_back(c);
	return m_archs.back();
}
