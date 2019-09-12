#ifndef CMOVE_HH
#define CMOVE_HH

#include <SFML/System/Vector2.hpp>

class	CMove
{
public:

	CMove(const sf::Vector2f& move) noexcept : m_move(move) {}

	const sf::Vector2f&	getMove() const noexcept { return m_move; }

private:

	sf::Vector2f	m_move;
};

#endif
