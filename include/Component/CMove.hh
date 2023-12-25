#pragma once

#include <SFML/System/Vector2.hpp>

// This component handles voluntary moving only.
struct CMove
{
	CMove(unsigned short speed) : m_speed(speed) {}

	bool isMoving() const { return m_velocity != sf::Vector2f(); }
	const auto& velocity() const { return m_velocity; }

	void setMotion(int direction) { m_velocity.x = m_speed * direction; }

private:

	const unsigned short m_speed;

	sf::Vector2f m_velocity;
};
