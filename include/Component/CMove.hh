#pragma once

#include <SFML/System/Vector2.hpp>

// This component handles voluntary moving only.
struct CMove
{
	CMove(unsigned short speed) : m_speed(speed) {}

	auto& velocity() const { return m_velocity; }

	void setMotionX(int direction);

private:

	sf::Vector2f m_velocity;

	unsigned short m_speed;
};
