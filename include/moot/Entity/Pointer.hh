#pragma once

#include <moot/Component/Collection.hh>
#include <moot/util/InstanceCounter.hh>
#include <limits>

struct EntityPointer : InstanceCounter<EntityPointer>
{
	EntityPointer() : collection(nullptr), index(std::numeric_limits<unsigned>::max()) {}
	EntityPointer(ComponentCollection* cc, unsigned idx) : collection(cc), index(idx) {}

	bool isValid() const { return collection; }

	template<typename C> const C& get() const noexcept requires (!std::is_pointer_v<C>) {
		return collection->getAll<C>()[index];
	}

	template<typename CP> CP get() const noexcept requires (std::is_pointer_v<CP>)
	{
		using C = std::remove_pointer_t<CP>;
		return &collection->getAll<C>()[index];
	}

	template<typename C> bool has() const
	{
		return collection->has<C>();
	}

	template<typename CP> CP find() const noexcept
	{
		using C = std::remove_pointer_t<CP>;
		if (has<C>())
			return get<CP>();
		assert(isValid());
		return nullptr;
	}

	ComponentComposition comp() const
	{
		return collection->comp();
	}

	ComponentCollection* collection;
	unsigned index;
};
