#pragma once

#include <moot/Component/Composable.hh>
#include <moot/util/InstanceCounter.hh>

class Archetype;

// An entity context holds a temporary pointer to an entity.
class EntityContext : InstanceCounter<EntityContext>
{
	friend class World;
	friend struct std::hash<EntityContext>;

public:

	EntityContext() : m_arch(nullptr), m_idx(0) {}
	EntityContext(Archetype* arch, unsigned index) : m_arch(arch), m_idx(index) {}	

	bool isEmpty() const { return !m_arch; }

	bool operator==(const EntityContext& right) const { return m_arch == right.m_arch && m_idx == right.m_idx; }
	bool operator<(const EntityContext& right) const { return m_arch < right.m_arch || m_idx < right.m_idx; }
	bool operator>(const EntityContext& right) const { return m_arch > right.m_arch || m_idx > right.m_idx; }

protected:

	Archetype* m_arch;
	unsigned m_idx;

private:

	bool operator<=(const EntityContext& right) const;
	bool operator>=(const EntityContext& right) const;
};
