#include <Archetype.hh>
#include <Template.hh>
#include <iostream>
#include <boost/mp11.hpp>

// Calls f with a variant run-time index turned into a compile-time one.
template <typename T, typename F>
static constexpr void	variantConstIndex(std::size_t index, F&& f)
{
	boost::mp11::mp_with_index<std::variant_size_v<T>>(index, f);
}

Archetype::Archetype(ComponentComposition comp) : ComponentComposable(comp)
{
// prepares the archetype for storing all needed components

	ComponentComposition::Bits bits = comp.bits();

// One bit set in comp means one component.
	m_entities.reserve(__builtin_popcount(bits));

// Iterate through all bits set in comp from low to high, creating the corresponding component vector.
	while (bits)
	{
		auto	variantIndex = __builtin_ctz(bits);
		variantConstIndex<ComponentVectorVariant>(variantIndex,
			[this](auto I)
			{
				m_entities.emplace_back(std::in_place_index<I>);
			});
		bits ^= 1 << variantIndex;
	}

// there should be some optimization here to avoid future reallocations
// using reserve() on all vectors depending on the composition
// __builtin_ctz() will be of use

	std::cout << "Archetype of size " << m_entities.size() << std::endl;
	if (__builtin_popcount(m_comp.bits()) != m_entities.size())
		asm("int $3");
}

void	Archetype::instantiate(const Template& temp)
{
	if (!(temp.comp() == m_comp))
		asm("int $3");

	// Instantiate a new entity by appending all of the template's components.
	unsigned i = 0;
 	for (const Template::ComponentVariant& component : temp.components())
	{
		variantConstIndex<Template::ComponentVariant>(component.index(),
			[&](auto I)
			{
				using ComponentType = std::variant_alternative_t<I, Template::ComponentVariant>;
				auto&	vec = std::get<std::vector<ComponentType>>(m_entities[i]);
				auto&	comp = std::get<ComponentType>(component);
				vec.push_back(comp);
			});
		++i;
	}
}
