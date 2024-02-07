#pragma once

#include <variant>
#include <boost/mp11.hpp>

// Calls f with a variant run-time index turned into a compile-time one.
template <typename T, typename F>
inline constexpr auto variantIndexToCompileTime(std::size_t index, F&& f)
{
	return boost::mp11::mp_with_index<std::variant_size_v<T>>(index, f);
}
