#pragma once

#include <SFML/System/Vector2.hpp>

// This component handles voluntary moving only.
struct CMove
{
	enum class Space
	{
		World,
		Camera
	};

	CMove(float speed) : m_speed(speed) {}

	auto& velocity() const { return m_velocity; }
	auto space() const { return m_space; }

	void setMotion(const sf::Vector2f& direction, Space);

private:

	sf::Vector2f m_velocity;
	Space m_space = Space::World;

	float m_speed;
};
