#pragma once

#include <moot/Component/Composable.hh>
#include <moot/Component/Variant.hh>
#include <map>
#include <type_traits>

class Template : public ComponentComposable
{
public:

	// Adds a (not already present) component.
	void add(ComponentVariant&&);
	template<typename C, typename...Args> C* add(Args&&... args)
	{
		return &std::get<C>(add(CId<C>, std::in_place_type<C>, std::forward<Args>(args)...));
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

	template<typename...Args> ComponentVariant& add(ComponentId cid, Args&&... args)
	{
		m_comp += cid;
		const auto& insertion = m_components.try_emplace(cid, std::forward<Args>(args)...);
		assert(insertion.second);
		return insertion.first->second;
	}

	// A map allows easily inserting mutable components in their sorting order without knowing in advance the final composition.
	std::map<ComponentId, ComponentVariant> m_components;

	friend class Archetype;
};
