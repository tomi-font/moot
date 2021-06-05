#pragma once

#include <Component/Component.hh>

class ComponentComposable
{
public:

	ComponentComposable(ComponentComposition comp) : m_comp(comp) {}
	virtual ~ComponentComposable() {}

	ComponentComposition	comp() const { return m_comp; }

protected:

	ComponentComposition	m_comp;
};
