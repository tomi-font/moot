#pragma once

#include <Component/Composition.hh>
#include <Component/Types.hh>

class ComponentComposable
{
public:

	ComponentComposable(ComponentComposition comp = ComponentComposition()) : m_comp(comp) {}
	virtual ~ComponentComposable() {}

	ComponentComposition comp() const { return m_comp; }
	template<typename C> constexpr bool has() const { return m_comp.hasAllOf(CId<C>); }

protected:

	ComponentComposition m_comp;
};
