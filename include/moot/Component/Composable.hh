#pragma once

#include <moot/Component/Composition.hh>
#include <moot/Component/Types.hh>

class ComponentComposable
{
public:

	constexpr ComponentComposable(ComponentComposition comp = ComponentComposition()) : m_comp(comp) {}

	constexpr ComponentComposition comp() const { return m_comp; }

	constexpr bool has(ComponentId cId) const { return m_comp.has(cId); }
	template<typename C> constexpr bool has() const { return has(CId<C>); }

	template<typename ...Cs> constexpr bool hasNoneOf() const { return m_comp.hasNoneOf((CId<Cs> | ...)); }

protected:

	ComponentComposition m_comp;
};
