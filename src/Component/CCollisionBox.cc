#include <Component/CCollisionBox.hh>

void CCollisionBox::resize(const sf::Vector2f& size)
{
    width = size.x;
    height = size.y;
}
