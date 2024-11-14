#pragma once

#include <moot/Entity/Pointer.hh>
#include <functional>

template<> struct std::hash<EntityPointer>
{
	std::size_t operator()(const EntityPointer&) const;
};

template<> struct std::hash<class Entity> : std::hash<EntityPointer> {};
