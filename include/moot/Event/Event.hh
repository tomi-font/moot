#pragma once

#include <moot/Entity/Context.hh>

struct Event
{
	enum Type
	{
		EntityMoved,
		GameClose,
		COUNT // Keep the enum values unassigned so that they are iterable.
	};

	const Type type;

	// The entity relevant to this event, if applicable.
	const EntityContext entity;
};
