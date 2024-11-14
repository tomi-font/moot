#include <moot/Component/Composable.hh>
#include <moot/Entity/Archetype.hh>
#include <moot/Entity/Prototype.hh>
#include <moot/util/variant/indexToCompileTime.hh>

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

EntityPointer Archetype::instantiate(const Prototype& entity)
{
	assert(entity.comp() == m_comp);
	ComponentComposition componentsLeft = m_comp;

	// Instantiate a new entity by appending all of the prototype's components.
	unsigned i = 0;
	for (const auto& [cid, component] : entity.m_components)
	{
		// Ensure that the prototype's components are stored in the expected order (ascending index).
		assert(componentsLeft.hasNoneOf((1 << cid) - 1));
		componentsLeft -= cid;

		assert(cid == component.index());
		variantIndexToCompileTime<ComponentVariant>(cid,
			[&](auto I)
			{
				using C = std::variant_alternative_t<I, ComponentVariant>;
				auto& vec = std::get<std::vector<C>>(m_entities[i]);
				auto& comp = std::get<C>(component);
				vec.push_back(comp);
			});
		++i;
	}
	
	const auto index = m_entityCount;
	++m_entityCount;
	return {this, index};
}

void Archetype::remove(const unsigned index)
{
	assert(index < m_entityCount);

	for (ComponentVectorVariant& componentVector : m_entities)
	{
		std::visit(
			[index](auto& compVec)
			{
				compVec.erase(compVec.begin() + index);
			},
			componentVector);
	}
	--m_entityCount;
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
