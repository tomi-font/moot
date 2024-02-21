#include <Entity/Handle.hh>

class EntityManipulator : public EntityHandle
{
public:

    EntityManipulator(EntityHandle&& entity) : EntityHandle(std::move(entity)) {}

    void resize(const sf::Vector2f&);
};
