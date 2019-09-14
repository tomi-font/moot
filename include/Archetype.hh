#ifndef ARCHETYPE_HH
#define ARCHETYPE_HH

#include "Component/Component.hh"
#include <vector>

// Archetypes store components of entities that are the same,
// i.e. possess exactly the same components

class Archetype
{
public:

	Archetype(CsComp);
// we don't want to copy construct archetypes
	Archetype(const Archetype&) {}

	~Archetype();

	CsComp	getComp() const noexcept
	{
		return m_comp;
	}

// returns the vector containing the component type requested
// behavior is undefined if the caller asks for some component that's not present
	template<typename T>
	std::vector<T>&	get() noexcept
	{
		// we calculate the index in m_cs by finding out how many bits are set up to C(T::Type),
		// as components will always be stored in the same order (the one defined in Component::Type enum)
		// C(T::Type)-1 so we can mask with all lower bits set, and then call __builtin_popcount() to count the bits
		// the rest is just a fancy cast
		return *reinterpret_cast<std::vector<T>*>(&m_cs[__builtin_popcount(m_comp & (C(T::Type) - 1))]);
	}

private:

// components composition for this archetype
	CsComp	m_comp;

// components of the same type are stored contiguously ; one vector for each component
// indeed, this is a massacre of OOP, C++, anything you want, but it remains safe
// as we know what's in there thanks to m_comp, and it's more performant/flexible than std::any
	std::vector<void*>*	m_cs;
};

#endif
