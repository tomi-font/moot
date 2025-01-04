#pragma once

#include <moot/Component/Collection.hh>

class Prototype : public ComponentCollection
{
public:

	template<typename C> inline const C& get() const
	{
		return getAll<C>().front();
	}
};
