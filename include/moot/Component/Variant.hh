#pragma once

#include <moot/Component/Types.hh>
#include <moot/util/tuple/toVariant.hh>

// Variant that can hold every component.
using ComponentVariant = tupleToVariant<Components>::type;

// Returns the ID of the given component.
constexpr ComponentId CVId(const ComponentVariant& cv)
{
	return std::visit([](const auto& component)
	{
		return CId<std::remove_cvref_t<decltype(component)>>;
	}, cv);
}

constexpr bool operator<(const ComponentVariant& lhs, const ComponentVariant& rhs)
{
	return CVId(lhs) < CVId(rhs);
}
