#pragma once

#include <moot/struct/Color.hh>
#include <algorithm>

struct FloatColor
{
	FloatColor() : r(0), g(0), b(0) {}
	FloatColor(sf::Color color) : r(color.r), g(color.g), b(color.b) {}

	operator Color() const
	{
		return {
			std::uint8_t(std::min(r, 255.f)),
			std::uint8_t(std::min(g, 255.f)),
			std::uint8_t(std::min(b, 255.f)),
		};
	}

	bool operator==(const FloatColor&) const = default;
	explicit operator bool() const { return *this != FloatColor(); }

	FloatColor& operator*=(float ratio)
	{
		r *= ratio;
		g *= ratio;
		b *= ratio;
		return *this;
	}
	FloatColor operator*(float ratio) const { return FloatColor(*this) *= ratio; }

	FloatColor& operator+=(const FloatColor& other)
	{
		r += other.r;
		g += other.g;
		b += other.b;
		return *this;
	}
	FloatColor operator+(const FloatColor& other) const { return FloatColor(*this) += other; }

	FloatColor& operator-=(const FloatColor& other)
	{
		r -= other.r;
		g -= other.g;
		b -= other.b;
		return *this;
	}
	FloatColor operator-(const FloatColor& other) const { return FloatColor(*this) -= other; }

	float r;
	float g;
	float b;
};
