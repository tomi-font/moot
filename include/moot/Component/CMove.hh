#pragma once

#include <SFML/System/Vector2.hpp>

// This component handles voluntary moving only.
struct CMove
{
	CMove(float speed) : m_speed(speed) {}

	auto& velocity() const { return m_velocity; }

	// Moves at full speed in the given direction, or stops if it is zero.
	void setMotion(const sf::Vector2f& direction);

private:

	sf::Vector2f m_velocity;

	float m_speed;
};
