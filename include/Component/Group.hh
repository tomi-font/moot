#ifndef COMPONENT_GROUP_HH
#define COMPONENT_GROUP_HH

#include "Component.hh"
#include "../Archetype.hh"

// Groups are the way systems express their interest towards different Components

struct	ComponentGroup
{
	ComponentGroup(CsComp required, CsComp forbidden = 0) : inc(required), exc(forbidden) {}

// vector of archetypes matching the conditions
	std::vector<Archetype*>	archs;

// components whose presence is required
	CsComp	inc;
// components whose presence is forbidden
	CsComp	exc;
};

#endif
