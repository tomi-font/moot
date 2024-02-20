#include <Component/Composable.hh>
#include <Entity/Archetype.hh>
#include <Entity/Template.hh>
#include <utility/variant/indexToCompileTime.hh>

Archetype::Archetype(ComponentComposition comp, World* world) :
	ComponentComposable(comp),
	m_world(world),
	m_entityCount(0)
{
	// Prepare the Archetype for storing all needed components.

	m_entities.reserve(comp.count());

	// Create the corresponding vector for every component present.
	for (ComponentId cid : comp)
	{
		variantIndexToCompileTime<ComponentVectorVariant>(cid,
			[this](auto I)
			{
				m_entities.emplace_back(std::in_place_index<I>);
			});
	}
}

void Archetype::instantiate(const Template& temp)
{
	assert(temp.comp() == m_comp);
	ComponentComposition componentsLeft = m_comp;

	// Instantiate a new entity by appending all of the template's components.
	unsigned i = 0;
 	for (const Template::ComponentVariant& component : temp.components())
	{
		// Ensure that the template's components are stored in the expected order (ascending index).
		const ComponentId cid = static_cast<ComponentId>(component.index());
		assert(componentsLeft.hasNoneOf((1 << cid) - 1));
		componentsLeft -= cid;

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
	++m_entityCount;
}

unsigned Archetype::computeEntityCount() const
{
	const unsigned firstVariantIndex = *m_comp.begin();

	return variantIndexToCompileTime<ComponentVectorVariant>(firstVariantIndex,
		[this](auto I)
		{
			return static_cast<unsigned>(std::get<I>(m_entities[0]).size());
		});
}
