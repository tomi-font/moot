#ifndef CTRANSFORM_HH
#define CTRANSFORM_HH

#include "Component.hh"
#include <SFML/System/Vector2.hpp>

class	CTransform : public Component
{
private:

	sf::Vector2f	m_pos;
};

#endif

