#pragma once

#include <tuple>
#include <variant>
#include <vector>

// Helper struct to convert a tuple into a variant of vectors.
template<typename Tuple> struct tupleToVectorVariant;
template<typename... Ts> struct tupleToVectorVariant<std::tuple<Ts...>> { using type = std::variant<std::vector<Ts>...>; };
