#pragma once

#include <moot/Entity/Pointer.hh>

struct Event
{
	enum Type
	{
		GameClose,
		COUNT // Keep the enum values unassigned so that they are iterable.
	};

	const Type type;
};
