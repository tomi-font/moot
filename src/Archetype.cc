#include "Component/Composable.hh"
#include <Archetype.hh>
#include <Template.hh>
#include <utility/variant/indexToCompileTime.hh>
#include <iostream>

Archetype::Archetype(ComponentComposition comp) : ComponentComposable(comp)
{
// prepares the archetype for storing all needed components

	ComponentComposition::Bits bits = comp.bits();

	// One bit set in comp means one component type.
	m_entities.reserve(__builtin_popcount(bits));

	// Iterate through all bits set in comp from low to high, creating the corresponding component vector.
	while (bits)
	{
		const unsigned variantIndex = __builtin_ctz(bits);

		variantIndexToCompileTime<ComponentVectorVariant>(variantIndex,
			[this](auto I)
			{
				m_entities.emplace_back(std::in_place_index<I>);
			});

		bits ^= 1 << variantIndex;
	}

	assert(__builtin_popcount(m_comp.bits()) == m_entities.size());
}

void Archetype::instantiate(const Template& temp)
{
	assert(temp.comp() == m_comp);

	// Instantiate a new entity by appending all of the template's components.
	unsigned i = 0;
 	for (const Template::ComponentVariant& component : temp.components())
	{
		// Ensure that the template's components are stored in the expected order (ascending index).
		assert(i == __builtin_popcount(m_comp.bits() & ((1 << component.index()) - 1)));

		variantIndexToCompileTime<Template::ComponentVariant>(component.index(),
			[&](auto I)
			{
				using ComponentType = std::variant_alternative_t<I, Template::ComponentVariant>;
				auto& vec = std::get<std::vector<ComponentType>>(m_entities[i]);
				auto& comp = std::get<ComponentType>(component);
				vec.push_back(comp);
			});
		++i;
	}
}
