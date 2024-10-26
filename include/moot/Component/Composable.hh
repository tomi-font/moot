#pragma once

#include <moot/Component/Composition.hh>
#include <moot/Component/Types.hh>

class ComponentComposable
{
public:

	constexpr ComponentComposable(ComponentComposition comp = ComponentComposition()) : m_comp(comp) {}

	constexpr ComponentComposition comp() const { return m_comp; }

	constexpr bool has(ComponentId cid) const { return m_comp.has(cid); }
	template<typename C> constexpr bool has() const { return has(CId<C>); }

	constexpr bool hasNoneOf(ComponentComposition comp) const { return m_comp.hasNoneOf(comp); }

protected:

	ComponentComposition m_comp;
};
