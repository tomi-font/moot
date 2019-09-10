#ifndef CTRANSFORM_HH
#define CTRANSFORM_HH

#include <SFML/System/Vector2.hpp>

class	CTransform
{
public:

	CTransform(const sf::Vector2f& pos) : m_pos(pos) {}

private:

	sf::Vector2f	m_pos;
};

#endif

