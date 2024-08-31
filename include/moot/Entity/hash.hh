#pragma once

#include <moot/Entity/Context.hh>

template<> struct std::hash<EntityContext>
{
	std::size_t operator()(const EntityContext&) const;
};

template<> struct std::hash<Entity> : std::hash<EntityContext> {};
