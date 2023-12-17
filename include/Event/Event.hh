#pragma once

#include <Entity/Id.hh>

class Event
{
public:

	enum Type
	{
		// Keep the enum's values unassigned, so that it can be iterated.
		PlayerQuit,
		COUNT
	};

	Event(Type t, EntityId eid = {}) : type(t), entityId(eid) {}

	const Type type;
	// The ID of the entity relevant to this event, if applicable.
	const EntityId entityId;
};
