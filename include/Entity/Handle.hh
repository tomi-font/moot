#pragma once

#include <Entity/Id.hh>
#include <Entity/Archetype.hh>

class EntityHandle
{
public:

    EntityHandle(Archetype* arch, unsigned index) : m_arch(arch), m_idx(index) {}

    operator bool() const { return m_arch; }

    template<typename C> bool has() const
    {
        assert(*this);
        return m_arch->has<C>();
    }

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

    EntityId id() const { return { m_arch->comp(), m_idx }; }

private:

    Archetype* const m_arch;
    const unsigned m_idx;
};
