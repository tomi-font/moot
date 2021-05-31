#pragma once

#include <Component/Component.hh>
#include <Archetype.hh>

// Groups are the way systems express their interest towards different component compositions.

struct ComponentGroup
{
	ComponentGroup(CsComp required, CsComp forbidden = CsComp()) : inc(required), exc(forbidden) {}
	
// The composition matches if all of the included and none of the excluded components are present.
	bool	matches(CsComp comp) { return (comp & inc) == inc && !(comp & exc).bits(); }

// Appends the archetype upon match.
	void	match(Archetype*);

// Matching archetypes.
	std::vector<Archetype*>	archs;

private:

// Components whose presence is required.
	CsComp	inc;
// Components whose presence is forbidden.
	CsComp	exc;
};
