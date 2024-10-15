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

	Event(Type type, EntityContext entity = {}) : type(type), entity(entity) {}

	const Type type;

	// The entity relevant to this event, if applicable.
	const EntityContext entity;
};
