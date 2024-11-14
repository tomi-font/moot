#pragma once

#include <moot/Component/Types.hh>
#include <moot/util/tuple/toVariant.hh>

// Variant that can hold every component.
using ComponentVariant = tupleToVariant<Components>::type;

// Returns the ID of the given component.
constexpr ComponentId CVId(const ComponentVariant& component)
{
	return static_cast<ComponentId>(component.index());
}

constexpr bool operator<(const ComponentVariant& lhs, const ComponentVariant& rhs)
{
	return CVId(lhs) < CVId(rhs);
}
