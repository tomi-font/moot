#pragma once

#include <moot/Component/Composable.hh>
#include <moot/utility/InstanceCounter.hh>

class Archetype;

// An entity context holds a temporary pointer to an entity as well as its component composition.
class EntityContext : public ComponentComposable, InstanceCounter<EntityContext>
{
	friend class World;
	friend struct std::hash<EntityContext>;

public:

	EntityContext() : m_arch(nullptr), m_idx(0) { assert(!*this); }
	EntityContext(ComponentComposition comp, Archetype* arch, unsigned index) : ComponentComposable(comp), m_arch(arch), m_idx(index) {}	

	operator bool() const { return m_arch; }

	// The component composition is not compared because it may differ between contexts of a same entity.
	bool operator==(const EntityContext& right) const { return m_arch == right.m_arch && m_idx == right.m_idx; }
	bool operator<(const EntityContext& right) const { return m_arch < right.m_arch || m_idx < right.m_idx; }
	bool operator>(const EntityContext& right) const { return m_arch > right.m_arch || m_idx > right.m_idx; }

protected:

	Archetype* m_arch;
	unsigned m_idx;

	void destroy()
	{
		m_arch = nullptr;
		m_comp = {};
		assert(!*this);
	}

private:

	bool operator<=(const EntityContext& right) const;
	bool operator>=(const EntityContext& right) const;
};
