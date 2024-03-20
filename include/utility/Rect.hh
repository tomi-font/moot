#pragma once

#include <utility/math.hh>
#include <utility/Vector2.hh>
#include <algorithm>

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
struct Rect
{
	Rect() {}
	Rect(const sf::Vector2<T>& pos, const sf::Vector2<T>& size) : min(pos), size(size) { }

	const sf::Vector2<T>& position() const { return min; }

	T right() const { return left + width; }
	T top() const { return bottom + height; }
	Vector2<T> max() const { return min + size; }
	
	bool hasPositiveArea() const { return width > 0 && height > 0; }

	auto& operator+=(const sf::Vector2<T>& move)
	{
		min += move;
		return *this;
	}
	Rect<T> operator+(const sf::Vector2<T>& move) const { return Rect(*this) += move; }

	auto& operator-=(const sf::Vector2<T>& move)
	{
		min -= move;
		return *this;
	}

	void setRight(T rightCoord) { left = rightCoord - width; }
	void setTop(T topCoord) { bottom = topCoord - height; }

	Rect<T> intersectionWith(const Rect<T>& other) const
	{
		Rect<T> inter;
		inter.min.x  = std::max(min.x, other.min.x);
		inter.min.y  = std::max(min.y, other.min.y);
		inter.width  = std::min(max().x, other.max().x) - inter.min.x;
		inter.height = std::min(max().y, other.max().y) - inter.min.y;
		return inter;
	}

	bool intersects(const Rect<T>& other) const { return intersectionWith(other).hasPositiveArea(); }

	void assertIntersects(const Rect<T>& other, bool intersects = true) const
	{
		Rect<T> inter = intersectionWith(other);
		assert(inter.hasPositiveArea() == intersects);
	}

	union
	{
		sf::Vector2<T> min;
		struct
		{
			T left;
			T bottom;
		};
	};
	union
	{
		sf::Vector2<T> size;
		struct
		{
			T width;
			T height;
		};
	};
};

using FloatRect = Rect<float>;
