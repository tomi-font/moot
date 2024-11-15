#pragma once

#include <moot/Entity/Pointer.hh>

struct Event
{
	using Id = unsigned;

	const Id id;

	const EntityPointer entity;
};
