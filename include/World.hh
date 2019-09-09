#ifndef WORLD_HH
#define WORLD_HH

#include "Entity.hh"

class	World
{
public:

	World() : m_nextId(0) {}

	EntityId	createEntity()
	{
		return m_nextId++;
	}

private:

	EntityId	m_nextId;
};

#endif
