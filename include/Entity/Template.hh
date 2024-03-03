#pragma once

#include <Component/Composable.hh>
#include <Component/Variant.hh>
#include <set>

class Template : public ComponentComposable
{
public:

	// Adds a (not already present) component.
	Template& add(ComponentVariant&&);
	template<typename C, typename...Args> Template& add(Args&&... args)
	{
		m_comp += CId<C>;
		emplace(std::in_place_type<C>, std::forward<Args>(args)...);
		return *this;
	}

	// Removes a (present) component.
	void remove(ComponentId);

private:

	template<typename...Args> void emplace(Args&&... args)
	{
		const bool added = m_components.emplace(std::forward<Args>(args)...).second;
		assert(added);
	}

	// A set allows easily inserting components in their sorting order without knowing in advance the final composition.
	std::set<ComponentVariant> m_components;

	friend class Archetype;
};
