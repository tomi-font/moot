#include <Entity/Manipulator.hh>

void EntityManipulator::resize(const sf::Vector2f& size)
{
    if (has<CRender>())
        get<CRender*>()->resize(size);

    if (has<CCollisionBox>())
        get<CCollisionBox*>()->resize(size);
}
