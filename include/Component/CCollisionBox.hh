#pragma once

#include <SFML/Graphics/Rect.hpp>

struct CCollisionBox : sf::FloatRect
{
    void resize(const sf::Vector2f&);
};
