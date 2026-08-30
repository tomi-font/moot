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

template<typename ...Cs> class ComponentCollectionIterator;

// Iterates over entities.
template<> class ComponentCollectionIterator<EntityPointer> : public ComponentCollectionIteratorBase
{
public:
	using ComponentCollectionIteratorBase::ComponentCollectionIteratorBase;

	EntityPointer operator*() const { return {*m_collectionIt, m_idx}; }
};

// Iterates over components of the given types.
template<typename ...Cs> class ComponentCollectionIterator : public ComponentCollectionIteratorBase
{
	std::tuple<const std::vector<Cs>* ...> m_components;

public:
	using ComponentCollectionIteratorBase::ComponentCollectionIteratorBase;

	std::tuple<const Cs&...> operator*()
	{
		if (m_idx == 0)
			m_components = {&(*m_collectionIt)->template getAll<Cs>() ...};

		return {(*std::get<const std::vector<Cs>*>(m_components))[m_idx] ...};
	}
};

// Additionally yields a pointer to the iterated entity, by value, in front of its components.
template<typename ...Cs> class ComponentCollectionIterator<EntityPointer, Cs...> : public ComponentCollectionIterator<Cs...>
{
public:
	using ComponentCollectionIterator<Cs...>::ComponentCollectionIterator;

	std::tuple<EntityPointer, const Cs&...> operator*()
	{
		return std::tuple_cat(std::tuple<EntityPointer>({*this->m_collectionIt, this->m_idx}),
		                      ComponentCollectionIterator<Cs...>::operator*());
	}
};

// Allows range-based for loop iteration over components of a same type.
template<typename ...Cs> class ComponentCollectionIterable
{
	const std::vector<ComponentCollection*>& m_collections;

public:
	ComponentCollectionIterable(decltype(m_collections) collections) : m_collections(collections) {}

	ComponentCollectionIterator<Cs...> begin() const { return {m_collections.begin()}; }
	ComponentCollectionIterator<Cs...> end() const { return {m_collections.end()}; }
};
