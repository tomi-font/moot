#ifndef CPOSITION_HH
#define CPOSITION_HH

#include <SFML/System/Vector2.hpp>

class	CPosition : public sf::Vector2f
{
public:

	CPosition(const sf::Vector2f& pos) noexcept : sf::Vector2f(pos) {}
};

#endif

