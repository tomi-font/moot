#pragma once

#include <Entity/Archetype.hh>

class EntityHandle
{
public:

    EntityHandle() : m_arch(nullptr), m_idx(0) { assert(!*this); }
    EntityHandle(Archetype* arch, unsigned index) : m_arch(arch), m_idx(index) {}
    EntityHandle(EntityHandle&& entity) : EntityHandle(entity.m_arch, entity.m_idx) { entity.destroy(); }

    operator bool() const { return m_arch; }

    template<typename C> bool has() const
    {
        assert(*this);
        return m_arch->has<C>();
    }

    ComponentComposition comp() const { return m_arch->comp(); }

    // Returns a const reference to the requested component.
    template<typename C, typename = std::enable_if_t<!std::is_pointer_v<C>>>
    const C& get() const
    {
        return *get<C*>();
    }
    
    // Returns a non-const pointer to the requested component.
    template<typename CP, typename = std::enable_if_t<std::is_pointer_v<CP>>>
    CP get() const
    {
        using C = std::remove_pointer_t<CP>;
        assert(has<C>());
        return (&m_arch->getAll<C>()[m_idx]);
    }

    World* world() const { return m_arch->world(); }

protected:

    void destroy();

private:

    Archetype* m_arch;
    const unsigned m_idx;

    friend Archetype;
};
