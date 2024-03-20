#pragma once

#include <SFML/System/Vector2.hpp>

template <typename T> struct Vector2 : sf::Vector2<T>
{
	using sf::Vector2<T>::Vector2;
	Vector2(const sf::Vector2<T>& other) : sf::Vector2<T>(other) {}

	bool isZero() const { return *this == sf::Vector2<T>(); }
	bool isNotZero() const { return !isZero(); }
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
