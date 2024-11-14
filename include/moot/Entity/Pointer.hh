#pragma once

#include <moot/util/InstanceCounter.hh>

class Archetype;

class EntityPointer : public InstanceCounter<EntityPointer>
{
public:

	EntityPointer() : m_arch(nullptr), m_idx(0) {}
	EntityPointer(Archetype* arch, unsigned index) : m_arch(arch), m_idx(index) {}	

	bool isValid() const { return m_arch; }

	Archetype* arch() const { return m_arch; }
	unsigned index() const { return m_idx; }

	bool operator==(const EntityPointer& right) const { return m_arch == right.m_arch && m_idx == right.m_idx; }
	bool operator<(const EntityPointer& right) const { return m_arch < right.m_arch || m_idx < right.m_idx; }
	bool operator>(const EntityPointer& right) const { return m_arch > right.m_arch || m_idx > right.m_idx; }

protected:

	Archetype* m_arch;
	unsigned m_idx;

private:

	bool operator<=(const EntityPointer& right) const;
	bool operator>=(const EntityPointer& right) const;
};
