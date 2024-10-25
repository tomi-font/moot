#pragma once

#include <moot/utility/math.hh>
#include <SFML/System/Vector2.hpp>

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
struct Rect
{
	Rect() { left = bottom = width = height = {}; }
	Rect(const sf::Vector2<T>& pos, const sf::Vector2<T>& size) : left(pos.x), bottom(pos.y), width(size.x), height(size.y) {}
	Rect(T leftEdge, T bottomEdge, T rightEdge, T topEdge) : Rect({leftEdge, bottomEdge}, {rightEdge - leftEdge, topEdge - bottomEdge}) {}

	sf::Vector2<T> position() const { return {left, bottom}; }
	T right() const { return left + width; }
	T top() const { return bottom + height; }

	sf::Vector2<T> size() const { return {width, height}; }

	bool hasPositiveArea() const { return width > 0 && height > 0; }

	auto& operator+=(const sf::Vector2<T>& move)
	{
		left += move.x;
		bottom += move.y;
		return *this;
	}
	Rect<T> operator+(const sf::Vector2<T>& move) const { return Rect(*this) += move; }

	auto& operator-=(const sf::Vector2<T>& move)
	{
		left -= move.x;
		bottom -= move.y;
		return *this;
	}
	Rect<T> operator-(const sf::Vector2<T>& move) const { return Rect(*this) -= move; }

	void setPosition(const sf::Vector2<T>& pos)
	{
		left = pos.x;
		bottom = pos.y;
	}
	void setRight(T rightCoord) { left = rightCoord - width; }
	void setTop(T topCoord) { bottom = topCoord - height; }

	Rect<T> intersectionWith(const Rect<T>& other) const
	{
		return {
			std::max(left, other.left),
			std::max(bottom, other.bottom),
			std::min(right(), other.right()),
			std::min(top(), other.top())
		};
	}

	bool intersects(const Rect<T>& other) const { return intersectionWith(other).hasPositiveArea(); }

	void assertIntersects(const Rect<T>& other, bool intersects = true) const
	{
		Rect<T> inter = intersectionWith(other);
		assert(inter.hasPositiveArea() == intersects);
	}

	T left;
	T bottom;
	T width;
	T height;
};

using FloatRect = Rect<float>;
