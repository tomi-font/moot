#pragma once

#include <SFML/System/Vector2.hpp>

struct CPosition : sf::Vector2f
{
public:

	static constexpr auto	Type = Component::Position;

	CPosition(const sf::Vector2f& pos) noexcept : sf::Vector2f(pos) {}
};

