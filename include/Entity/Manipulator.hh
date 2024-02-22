#include <Entity/Handle.hh>
#include <World.hh>

class EntityManipulator : public EntityHandle
{
public:

    EntityManipulator(EntityHandle&& entity) : EntityHandle(std::move(entity)) {}

    void resize(const sf::Vector2f&);

    template<typename C> void remove() && 
    {
        assert(has<C>());
        
        // To remove component C, instantiate a new entity with the component removed...
        Template temp(*this);
        temp.remove<C>();
        world()->instantiate(std::move(temp));

        // ... And remove the entity that has it.
        world()->remove(std::move(*this));
        destroy();
    }
};
