#pragma once

#include <Component/Composition.hh>

class ComponentComposable
{
public:

	ComponentComposable(ComponentComposition comp = ComponentComposition()) : m_comp(comp) {}
	virtual ~ComponentComposable() {}

	ComponentComposition comp() const { return m_comp; }

protected:

	ComponentComposition m_comp;
};
