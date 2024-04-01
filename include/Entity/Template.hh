#pragma once

#include <Component/Composable.hh>
#include <Component/Variant.hh>
#include <map>

class Template : public ComponentComposable
{
public:

	// Adds a (not already present) component.
	void add(ComponentVariant&&);
	template<typename C, typename...Args> void add(Args&&... args)
	{
		add(CId<C>, std::in_place_type<C>, std::forward<Args>(args)...);
	}

	template<typename CP, typename = std::enable_if_t<std::is_pointer_v<CP>>>
	CP get()
	{
		using C = std::remove_pointer_t<CP>;
		assert(has<C>());
		return &std::get<C>(m_components.at(CId<C>));
	}
	template<typename C, typename = std::enable_if_t<!std::is_pointer_v<C>>>
	const C& get() const
	{
		assert(has<C>());
		return std::get<C>(m_components.at(CId<C>));
	}

	// Removes a (present) component.
	void remove(ComponentId);

private:

	template<typename...Args> void add(ComponentId cid, Args&&... args)
	{
		m_comp += cid;
		const bool added = m_components.try_emplace(cid, std::forward<Args>(args)...).second;
		assert(added);
	}

	// A map allows easily inserting mutable components in their sorting order without knowing in advance the final composition.
	std::map<ComponentId, ComponentVariant> m_components;

	friend class Archetype;
};
