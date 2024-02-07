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

    template<typename C> C& get() const
    {
        assert(has<C>());
        return m_arch->getAll<C>()[m_idx];
    }

    EntityId id() const { return { m_arch->comp(), m_idx }; }

private:

    Archetype* const m_arch;
    const unsigned m_idx;
};
