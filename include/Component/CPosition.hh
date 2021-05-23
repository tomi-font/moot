#ifndef CPOSITION_HH
#define CPOSITION_HH

#include <SFML/System/Vector2.hpp>

class	CPosition : public sf::Vector2f
{
public:

	static constexpr auto Type = Component::Position;

	CPosition(const sf::Vector2f& pos) noexcept : sf::Vector2f(pos) {}
};

#endif

