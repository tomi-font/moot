#pragma once

#include <Component/Types.hh>
#include <utility/tuple/toVariant.hh>

// Variant containing every component.
using ComponentVariant = tupleToVariant<Components>::type;

// Compares ComponentVariants according to their ComponentId.
constexpr bool operator<(const ComponentVariant& lhs, const ComponentVariant& rhs)
{
	return lhs.index() < rhs.index();
}
