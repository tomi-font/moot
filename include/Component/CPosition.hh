#ifndef CPOSITION_HH
#define CPOSITION_HH

#include <SFML/System/Vector2.hpp>

class	CPosition
{
public:

	CPosition(const sf::Vector2f& pos) noexcept : m_pos(pos) {}

private:

	sf::Vector2f	m_pos;
};

#endif

