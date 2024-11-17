#pragma once

#include <moot/Component/Composition.hh>

class ComponentIdRegistry
{
	static unsigned s_m_nextId;

public:

	template<typename C> static inline const ComponentComposition idOf = ComponentId(s_m_nextId++);

	static unsigned idCount() { return s_m_nextId; }
};

template<typename C> static inline const ComponentComposition& CId = ComponentIdRegistry::idOf<C>;
