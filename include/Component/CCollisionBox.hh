#ifndef CCOLLISIONBOX_HH
#define CCOLLISIONBOX_HH

#include <SFML/Graphics/Rect.hpp>

class	CCollisionBox : public sf::FloatRect
{
public:

	static constexpr auto Type = Component::CollisionBox;

	CCollisionBox(const sf::FloatRect& rect) noexcept : sf::FloatRect(rect) {}
};

#endif
