#pragma once

#include <Entity/Archetype.hh>

class ArchetypeIteratorBase
{
protected:
	std::vector<Archetype*>::const_iterator m_archIt;
	unsigned m_idx = 0;

public:
	ArchetypeIteratorBase(decltype(m_archIt) archIt) : m_archIt(archIt) {}
	bool operator==(const ArchetypeIteratorBase& other) const { return m_archIt == other.m_archIt; }
};

template<typename C> class ArchetypeIterator;

// Iterates over entities.
template<> class ArchetypeIterator<EntityContext> : public ArchetypeIteratorBase
{
public:
	using ArchetypeIteratorBase::ArchetypeIteratorBase;

	EntityContext operator*() const { return { (*m_archIt)->comp(), *m_archIt, m_idx }; }

	ArchetypeIterator& operator++()
	{
		if (m_idx + 1 < (*m_archIt)->entityCount())
			++m_idx;
		else
		{
			++m_archIt;
			m_idx = 0;
		}
		return *this;
	}
};

// Iterates over components of a same type.
template<typename C> class ArchetypeIterator : public ArchetypeIteratorBase
{
	std::span<C> m_components;

public:
	using ArchetypeIteratorBase::ArchetypeIteratorBase;

	C& operator*()
	{
		if (m_idx == 0)
			m_components = (*m_archIt)->template getAll<C>();
		return m_components[m_idx];
	}

	ArchetypeIterator& operator++()
	{
		if (m_idx + 1 < m_components.size())
			++m_idx;
		else
		{
			++m_archIt;
			m_idx = 0;
		}
		return *this;
	}
};

// Allows range-based for loop iteration over components of a same type.
template<typename C> class ArchetypeIterable
{
	const std::vector<Archetype*>& m_archs;

public:
	ArchetypeIterable(decltype(m_archs) archs) : m_archs(archs) {}

	ArchetypeIterator<C> begin() const { return { m_archs.begin() }; }
	ArchetypeIterator<C> end() const { return { m_archs.end() }; }
};
