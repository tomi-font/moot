#pragma once

#include <moot/Component/Collection.hh>
#include <moot/Entity/Pointer.hh>

class ComponentCollectionIteratorBase
{
protected:
	std::vector<ComponentCollection*>::const_iterator m_collectionIt;
	unsigned m_idx = 0;

public:
	ComponentCollectionIteratorBase(decltype(m_collectionIt) collectionIt) : m_collectionIt(collectionIt) {}

	bool operator==(ComponentCollectionIteratorBase& other);

	void operator++() { ++m_idx; }
};

template<typename C> class ComponentCollectionIterator;

// Iterates over entities.
template<> class ComponentCollectionIterator<EntityPointer> : public ComponentCollectionIteratorBase
{
public:
	using ComponentCollectionIteratorBase::ComponentCollectionIteratorBase;

	EntityPointer operator*() const { return {*m_collectionIt, m_idx}; }
};

// Iterates over components of a same type.
template<typename C> class ComponentCollectionIterator : public ComponentCollectionIteratorBase
{
	const std::vector<C>* m_components;

public:
	using ComponentCollectionIteratorBase::ComponentCollectionIteratorBase;

	C& operator*()
	{
		if (m_idx == 0)
		{
			m_components = (*m_collectionIt)->template getAll<C>();
			assert(m_components->size() == (*m_collectionIt)->size());
		}
		return (*m_components)[m_idx];
	}
};

// Allows range-based for loop iteration over components of a same type.
template<typename C> class ComponentCollectionIterable
{
	const std::vector<ComponentCollection*>& m_collections;

public:
	ComponentCollectionIterable(decltype(m_collections) collections) : m_collections(collections) {}

	ComponentCollectionIterator<C> begin() const { return {m_collections.begin()}; }
	ComponentCollectionIterator<C> end() const { return {m_collections.end()}; }
};
