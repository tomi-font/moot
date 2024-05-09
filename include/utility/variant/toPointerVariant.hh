#pragma once

#include <variant>

// Helper struct to convert a variant into a variant of pointers.
template<typename Variant> struct variantToPointerVariant;
template<typename... Ts> struct variantToPointerVariant<std::variant<Ts...>> { using type = std::variant<typename std::add_pointer_t<Ts>...>; };
