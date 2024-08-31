#pragma once

#include <moot/Entity/Context.hh>

struct Event
{
	enum Type
	{
		// Keep the enum values unassigned so that they are iterable.
		EntityMoved,
		COUNT
	};
	const Type type;

	// The entity relevant to this event, if applicable.
	const EntityContext entity;
};
