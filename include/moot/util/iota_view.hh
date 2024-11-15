#pragma once

#include <ranges>
#include <span>
#include <vector>

template<typename T> static constexpr
std::ranges::iota_view<T, T> iota_view(auto first, auto last)
{
	return std::ranges::iota_view<T, T>(static_cast<T>(first), static_cast<T>(last));
}

template<typename T, typename U> static constexpr
std::span<T> span(std::vector<T>* vec, std::ranges::iota_view<U, U> iota_view)
{
	return {vec->begin() + iota_view.front(), iota_view.size()};
}
