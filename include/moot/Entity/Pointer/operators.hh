#pragma once

#include <moot/Entity/Pointer.hh>

bool operator==(const EntityPointer&, const EntityPointer&);

template<> struct std::hash<EntityPointer>
{
	std::size_t operator()(const EntityPointer&) const;
};
