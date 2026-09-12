#pragma once

#include <SFML/Graphics/Color.hpp>

struct Color : public sf::Color
{
	constexpr Color() : sf::Color(0, 0, 0, 0) {}
	constexpr Color(const sf::Color& color) : sf::Color(color) {}
	using sf::Color::Color;
	
	constexpr operator bool() const { return *this != Color(); }

	constexpr Color operator*(float f) const { return {std::uint8_t(r * f), std::uint8_t(g * f), std::uint8_t(b * f)}; }
};
