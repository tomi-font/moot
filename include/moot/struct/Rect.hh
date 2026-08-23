#pragma once

#include <moot/struct/Vector2.hh>
#include <moot/util/math/base.hh>

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
struct Rect
{
	Rect() { left = bottom = width = height = {}; }
	Rect(const sf::Vector2<T>& bottomLeft, const sf::Vector2<T>& size) : bottomLeft(bottomLeft), size(size) {}
	Rect(T leftEdge, T bottomEdge, T rightEdge, T topEdge) : Rect({leftEdge, bottomEdge}, {rightEdge - leftEdge, topEdge - bottomEdge}) {}

	T right() const { return left + width; }
	T top() const { return bottom + height; }

	bool hasPositiveArea() const { return size.isMoreThanZero(); }
	bool isEmpty() const { return size.isZero(); }

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

	bool intersects(const Rect<T>& other) const
	{
		Rect<T> inter = intersectionWith(other);
		return inter.width > epsilon(maxAbs(left, right()), maxAbs(other.left, other.right()))
		    && inter.height > epsilon(maxAbs(bottom, top()), maxAbs(other.bottom, other.top()));
	}

	union
	{
		struct
		{
			T left;
			T bottom;
		};
		Vector2<T> bottomLeft;
	};
	union
	{
		struct
		{
			T width;
			T height;
		};
		Vector2<T> size;
	};
};

using FloatRect = Rect<float>;
