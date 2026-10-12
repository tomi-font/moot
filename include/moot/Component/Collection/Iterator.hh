#pragma once

#include <moot/Component/Collection.hh>
#include <moot/Entity/Pointer.hh>
#include <tuple>
#include <type_traits>

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

template<typename C> using AccessedComponent = std::conditional_t<std::is_pointer_v<C>, C, const C&>;

// Iterates over components of the given types.
template<typename ...Cs> class ComponentCollectionIterator : public ComponentCollectionIteratorBase
{
	template<typename C> using IteratedComponent = std::remove_pointer_t<C>;

	std::tuple<std::vector<IteratedComponent<Cs>>* ...> m_components;

	template<typename C> std::vector<IteratedComponent<C>>* findAll() const
	{
		ComponentCollection* const collection = *m_collectionIt;

		if constexpr (std::is_pointer_v<C>)
			if (!(collection)->has<IteratedComponent<C>>())
				return nullptr;

		return &collection->getAll<IteratedComponent<C>>();
	}

	template<typename C> AccessedComponent<C> access() const
	{
		auto* const components = std::get<std::vector<IteratedComponent<C>>*>(m_components);
		if constexpr (std::is_pointer_v<C>)
			return components ? &(*components)[m_idx] : nullptr;
		else
			return (*components)[m_idx];
	}

public:
	using ComponentCollectionIteratorBase::ComponentCollectionIteratorBase;

	std::tuple<AccessedComponent<Cs>...> operator*()
	{
		if (m_idx == 0)
			m_components = {findAll<Cs>()...};

		return {access<Cs>()...};
	}
};

// Additionally yields a pointer to the iterated entity, by value, in front of its components.
template<typename ...Cs> class ComponentCollectionIterator<EntityPointer, Cs...> : public ComponentCollectionIterator<Cs...>
{
public:
	using ComponentCollectionIterator<Cs...>::ComponentCollectionIterator;

	std::tuple<EntityPointer, AccessedComponent<Cs>...> operator*()
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
