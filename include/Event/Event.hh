#pragma once

class Event
{
public:

	enum Type
	{
		// Keep the enum's values unassigned, so that it can be iterated.
		PlayerQuit,
		COUNT
	};

	Event(Type t) : type(t) {}

	const Type type;
};
