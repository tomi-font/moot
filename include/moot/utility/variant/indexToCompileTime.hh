#pragma once

#include <variant>
#include <boost/mp11/detail/mp_with_index.hpp>

// Calls f with a variant run-time index turned into a compile-time one.
template <typename T, typename F> inline constexpr
decltype(auto) variantIndexToCompileTime(std::size_t index, F&& f)
{
	return boost::mp11::mp_with_index<std::variant_size_v<T>>(index, f);
}
