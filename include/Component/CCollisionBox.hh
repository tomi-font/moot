#ifndef CCOLLISIONBOX_HH
#define CCOLLISIONBOX_HH

#include <SFML/Graphics/Rect.hpp>

class	CCollisionBox : public sf::FloatRect
{
public:

	CCollisionBox(const sf::FloatRect& rect) noexcept : sf::FloatRect(rect) {}
};

#endif
