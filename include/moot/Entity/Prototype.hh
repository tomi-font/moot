#pragma once

#include <moot/Component/Collection.hh>

class Prototype : public ComponentCollection
{
public:

	template<typename C> const C& get() const requires (!std::is_pointer_v<C>)
	{
		return getAll<C>().front();
	}

	template<typename CP> CP get() requires (std::is_pointer_v<CP>)
	{
		using C = std::remove_pointer_t<CP>;
		return &getAll<C>().front();
	}
};
