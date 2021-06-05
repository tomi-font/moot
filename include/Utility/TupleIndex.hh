#pragma once

#include <tuple>

// Helper struct to get the index of a class in a tuple.
template<typename T, typename Tuple> struct TupleIndex;

// Instance matching the requested class.
template<typename T, typename... Types> struct TupleIndex<T, std::tuple<T, Types...>> { static constexpr auto value = 0; };

// Instances before the matching one. Each increments the value.
template<typename T, typename U, typename... Types> struct TupleIndex<T, std::tuple<U, Types...>> { static constexpr auto value = 1 + TupleIndex<T, std::tuple<Types...>>::value; };
