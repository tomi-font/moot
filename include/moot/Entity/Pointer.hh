#pragma once

#include <moot/Component/Collection.hh>
#include <moot/util/InstanceCounter.hh>

struct EntityPointer : InstanceCounter<EntityPointer>
{
	EntityPointer() : collection(nullptr), index(UINT_MAX) {}
	EntityPointer(ComponentCollection* cc, unsigned idx) : collection(cc), index(idx) {}

	bool isValid() const { return collection; }

	template<typename C, typename = std::enable_if_t<!std::is_pointer_v<C>>>
	inline const C& get() const noexcept
	{
		return collection->getAll<C>()[index];
	}

	template<typename CP, typename = std::enable_if_t<std::is_pointer_v<CP>>>
	inline CP get() const noexcept
	{
		using C = std::remove_pointer_t<CP>;
		return &collection->getAll<C>()[index];
	}

	template<typename C> inline bool has() const
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
