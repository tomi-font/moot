#ifndef ENTITY_HH
#define ENTITY_HH

#include <SFML/System/Vector2.hpp>

class	Entity
{
public:

	Entity(const sf::Vector2f& pos);
	~Entity();

private:
	
	sf::Vector2f	m_pos;
};

#endif
