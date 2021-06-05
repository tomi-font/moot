#pragma once

#include <Component/Component.hh>
#include <vector>

class Archetype;

// Groups are the way systems express their interest towards different component compositions.
struct ComponentGroup
{
	ComponentGroup(ComponentComposition required, ComponentComposition forbidden = ComponentComposition()) : inc(required), exc(forbidden) {}
	
// The composition matches if all of the included and none of the excluded components are present.
	bool	matches(ComponentComposition comp) { return (comp & inc) == inc && !(comp & exc).bits(); }

// Appends the archetype upon match.
	void	match(Archetype*);

// Matching archetypes.
	std::vector<Archetype*>	archs;

private:

// Components whose presence is required.
	ComponentComposition	inc;
// Components whose presence is forbidden.
	ComponentComposition	exc;
};
