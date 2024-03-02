#include <Entity/Entity.hh>

void Entity::resize(const sf::Vector2f& size)
{
	if (has<CRender>())
		get<CRender*>()->resize(size);

	if (has<CCollisionBox>())
		get<CCollisionBox*>()->resize(size);
}
