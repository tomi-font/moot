#pragma once

#include <Component/Composition.hh>
#include <Component/Types.hh>

class ComponentComposable
{
public:

	ComponentComposable(ComponentComposition comp = ComponentComposition()) : m_comp(comp) {}

	ComponentComposition comp() const { return m_comp; }
	template<typename C> constexpr bool has() const { return m_comp.has(CId<C>); }
	constexpr bool has(ComponentId cid) const { return m_comp.has(cid); }

protected:

	ComponentComposition m_comp;
};
