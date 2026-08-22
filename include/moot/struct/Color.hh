#pragma once

#include <SFML/Graphics/Color.hpp>

struct Color : public sf::Color
{
	constexpr Color() : sf::Color(0, 0, 0, 0) {}
	constexpr Color(const sf::Color& color) : sf::Color(color) {}
	using sf::Color::Color;
	
	constexpr operator bool() const { return *this != Color(); }
};
