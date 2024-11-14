#pragma once

#include <SFML/Graphics/Color.hpp>

struct Color : public sf::Color
{
	Color() : sf::Color(0) {}
	Color(const sf::Color& color) : sf::Color(color) {}
	using sf::Color::Color;

	operator bool() const { return *this != Color(); }
};
