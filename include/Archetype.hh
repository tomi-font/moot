#ifndef ARCHETYPE_HH
#define ARCHETYPE_HH

#include "Component/Component.hh"
#include <vector>
#include <experimental/any>

// Archetypes store components of entities that are the same,
// i.e. possess exactly the same components

class Archetype
{
public:

	Archetype(t_Comp);

	t_Comp	getComp() const noexcept
	{
		return m_comp;
	}

// returns the vector containing the component type requested
// assumes the caller only asks for present components
	template<typename T>
	std::vector<T>&	get() noexcept
	{
		for (auto& c : m_cs)
		{
			if (c.type() == typeid(std::vector<T>))
				return std::experimental::any_cast<std::vector<T>&>(c);
		}
		#pragma GCC diagnostic ignored "-Wreturn-type"
	}

private:

// components of the same type are stored contiguously ; one vector for each component
	std::vector<std::experimental::any>	m_cs;

// components composition for this archetype
	t_Comp	m_comp;
};

#endif
