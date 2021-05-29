#pragma once

#include <SFML/System/Vector2.hpp>

// this component represents voluntary moving only
// external forces are handled by the rigidbody component
class	CMove
{
public:

	static constexpr auto Type = Component::Move;

	CMove(unsigned short speed) noexcept : m_moving(false), m_moved(false), m_speed(speed) {}

	const sf::Vector2f&	getVelocity() const noexcept { return m_velocity; }
	void				setVelocity(const sf::Vector2f& velocity) noexcept { m_velocity = velocity; }

	bool	isMoving() const noexcept { return m_moving; }
	void	setMoving(bool moving) noexcept
	{
		m_moving = moving;
		m_moved = moving;	// <-- this may bring an issue at some point:
	}						// if setMoving(true) and some system calls hasMoved() before the physics system updates, the position won't have changed

	bool	hasMoved() const noexcept { return m_moved; }
	bool	setMoved(bool moved) noexcept { m_moved = moved; return moved; }

	unsigned short	getSpeed() const noexcept { return m_speed; }

private:

	sf::Vector2f	m_velocity;

// is the entity moving? (used by physics system)
// if set to true, assumes a non-zero velocity
	bool			m_moving;

// has the entity moved since last frame? (used by other systems)
	bool			m_moved;

	unsigned short	m_speed;
};
