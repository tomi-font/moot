#pragma once

#include <SFML/System/Vector2.hpp>
#include <algorithm>

template<typename T> struct Vector2 : sf::Vector2<T>
{
	using sf::Vector2<T>::Vector2;
	Vector2(const sf::Vector2<T>& other) : sf::Vector2<T>(other) {}

	bool isZero() const { return *this == sf::Vector2<T>(); }
	bool isNotZero() const { return !isZero(); }
	bool isMoreThanZero() const { return min() > 0; }

	T min() const { return std::min(this->x, this->y); }
	T max() const { return std::max(this->x, this->y); }
};
template<typename T> Vector2(sf::Vector2<T>) -> Vector2<T>;

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2u = Vector2<unsigned>;

namespace sf
{
	template<typename T> sf::Vector2<T> operator/(const sf::Vector2<T>& left, const sf::Vector2<T>& right)
	{
		return {left.x / right.x, left.y / right.y};
	}

	template<typename T, typename U> sf::Vector2<T> operator*(U left, const sf::Vector2<T>& right)
	{
		return {left * right.x, left * right.y};
	}
}
