#pragma once

#include <SFML/System/Vector2.hpp>

class CRigidbody
{
public:

	void applyYForce(float force) { m_velocity.y += force; }
	
	void zeroVelocityX() { m_velocity.x = 0; }
	void zeroVelocityY() { m_velocity.y = 0; }

	auto& velocity() const { return m_velocity; }

private:

	sf::Vector2f m_velocity;
};
