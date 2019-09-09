#ifndef WORLD_HH
#define WORLD_HH

#include "Entity.hh"
#include "Component/CTransform.hh"

class	World
{
public:

	t_EntityId	createEntity()
	{
		return m_nextId++;
	}

private:

	t_EntityId	m_nextId = 0;
};

#endif
