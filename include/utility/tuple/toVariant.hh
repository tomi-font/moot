#pragma once

#include <tuple>
#include <variant>

// Helper struct to convert a tuple into a variant.
template<typename Tuple> struct tupleToVariant;
template<typename... Ts> struct tupleToVariant<std::tuple<Ts...>> { using type = std::variant<Ts...>; };
