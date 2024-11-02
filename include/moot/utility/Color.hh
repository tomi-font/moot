#pragma once

#include <SFML/Graphics/Color.hpp>

class Color : public sf::Color
{
public:

	Color() : sf::Color(0) {}
	Color(const sf::Color& color) : sf::Color(color) {}
	using sf::Color::Color;

	operator bool() const { return *this != Color(); }
};
