#ifndef WORLD_HH
#define WORLD_HH

#include "Entity.hh"
#include "Archetype.hh"

class	World
{
public:

// returns the archetype matching the components composition
// passed in argument, creating it if it didn't exist
	Archetype&	getArchetype(t_Comp);

	t_EntityId	createEntity() noexcept
	{
		return m_nextId++;
	}

private:

	t_EntityId	m_nextId = 0;

	std::vector<Archetype>	m_archs;
};

#endif
