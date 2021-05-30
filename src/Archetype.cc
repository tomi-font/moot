#include <Archetype.hh>
#include <iostream>
#include <boost/mp11.hpp>

template <typename T>
T	variant_from_index(size_t index)
{
	return boost::mp11::mp_with_index<std::variant_size_v<T>>(index,
			[](auto I){ return T(std::in_place_index<I>); });
}

Archetype::Archetype(CsComp comp) : m_comp(comp)
{
// prepares the archetype for storing all needed components

// one bit set in comp means one component
// thus, the number of set bits == number of vectors needed
	m_components.reserve(__builtin_popcount(comp));

// iterate through all bits set in comp, in increasing order
// emplacing the corresponding component vector
	while (comp)
	{
		unsigned	variantIndex = __builtin_ctz(comp);
		m_components.emplace_back(variant_from_index<ComponentVariant>(variantIndex));
		comp ^= 1 << variantIndex;
	}

// there should be some optimization here to avoid future reallocations
// using reserve() on all vectors depending on the composition
// __builtin_ctz() will be of use

	std::cout << "Archetype of size " << m_components.size() << std::endl;
	if (__builtin_popcount(m_comp) != m_components.size())
		asm("int $3");
}
