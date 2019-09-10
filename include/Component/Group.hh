#ifndef COMPONENT_GROUP_HH
#define COMPONENT_GROUP_HH

#include "Component.hh"
#include "../Archetype.hh"

// Groups are the way systems express their interest towards different Components

struct	ComponentGroup
{
	ComponentGroup(t_Comp required, t_Comp forbidden = 0) : inc(required), exc(forbidden) {}

// vector of archetypes matching the conditions
	std::vector<Archetype*>	archs;

// components whose presence is required
	t_Comp	inc;
// components whose presence is forbidden
	t_Comp	exc;
};

#endif
