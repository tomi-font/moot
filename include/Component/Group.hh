#ifndef COMPONENT_GROUP_HH
#define COMPONENT_GROUP_HH

#include "Component.hh"
#include "../Archetype.hh"

// Groups are the way systems express their interest towards different component compositions.

struct ComponentGroup
{
	ComponentGroup(CsComp required, CsComp forbidden = 0) : inc(required), exc(forbidden) {}
	
// The composition is matched if all of the included and none of the excluded components are present.
	bool matches(CsComp comp) { return (comp & inc) == inc && !(comp & exc); }

// Appends the archetype upon match.
	void match(Archetype*);

// Matching archetypes.
	std::vector<Archetype*>	archs;

private:

// Components whose presence is required.
	CsComp	inc;
// Components whose presence is forbidden.
	CsComp	exc;
};

#endif
