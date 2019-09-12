#ifndef CMOVE_HH
#define CMOVE_HH

#include <SFML/System/Vector2.hpp>

class	CMove
{
public:

	CMove() noexcept : m_moved(false) {}

	const sf::Vector2f&	getMove() const noexcept { return m_move; }
	void	setMove(const sf::Vector2f& move) noexcept { m_move = move; }

	bool	hasMoved() const noexcept { return m_moved; }
	void	setMoved(bool moved) noexcept { m_moved = moved; }

private:

	sf::Vector2f	m_move;

// stores whether the entity moved since last frame
	bool			m_moved;
};

#endif
