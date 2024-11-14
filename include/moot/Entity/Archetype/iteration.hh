#pragma once

#include <moot/Entity/Archetype.hh>
#include <moot/Entity/Pointer.hh>

class ArchetypeIteratorBase
{
protected:
	std::vector<Archetype*>::const_iterator m_archIt;
	unsigned m_idx = 0;

public:
	ArchetypeIteratorBase(decltype(m_archIt) archIt) : m_archIt(archIt) {}

	bool operator==(ArchetypeIteratorBase& other) &
	{
		while (m_archIt != other.m_archIt)
		{
			if (m_idx != (*m_archIt)->entityCount())
				return false;

			++m_archIt;
			m_idx = 0;
		}
		return true;
	}
	bool operator==(ArchetypeIteratorBase&& other) && { return *this == other; }

	void operator++() { ++m_idx; }
};

template<typename C> class ArchetypeIterator;

// Iterates over entities.
template<> class ArchetypeIterator<EntityPointer> : public ArchetypeIteratorBase
{
public:
	using ArchetypeIteratorBase::ArchetypeIteratorBase;

	EntityPointer operator*() const { return { *m_archIt, m_idx }; }
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
		{
			m_components = (*m_archIt)->template getAll<C>();
			assert(m_components.size() == (*m_archIt)->entityCount());
		}
		return m_components[m_idx];
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
