#pragma once

#include <SFML/System/Vector2.hpp>

template<typename T> struct Vector2 : sf::Vector2<T>
{
	using sf::Vector2<T>::Vector2;
	Vector2(const sf::Vector2<T>& other) : sf::Vector2<T>(other) {}

	bool isZero() const { return *this == sf::Vector2<T>(); }
	bool isNotZero() const { return !isZero(); }

	T min() const { return std::min(this->x, this->y); }
	T max() const { return std::max(this->x, this->y); }
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;

namespace sf
{
	template<typename T> sf::Vector2<T> operator/(const sf::Vector2<T>& right, const sf::Vector2<T>& left)
	{
		return {right.x / left.x, right.y / left.y};
	}
}
