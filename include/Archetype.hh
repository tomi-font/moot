#pragma once

#include <Component/Component.hh>
#include <Component/Types.hh>
#include <vector>
#include <variant>

// Archetypes store components of entities that are the same, i.e. have exactly the same components.
class Archetype
{
public:

	Archetype(ComponentComposition);

	Archetype(const Archetype&) = delete;
	void	operator=(const Archetype&) = delete;

	ComponentComposition	comp() const noexcept { return m_comp; }

// returns the vector containing the component type requested
// behavior is undefined if the caller asks for some component that's not present
	template<typename T>
	std::vector<T>&	get() noexcept
	{
		// we calculate the index in m_components by finding out how many bits are set up to
		// C(T::Type), as components will always be stored in the ComponentVariant order
		return std::get<std::vector<T>>(m_components[__builtin_popcount(m_comp.bits() & (CId<T>.bits() - 1))]);
	}

private:

// Components composing this archetype.
	ComponentComposition	m_comp;

// Helper struct to convert a tuple into a variant of vectors.
	template<typename Tuple> struct VectorVariantGetter;
	template<typename... Ts> struct VectorVariantGetter<std::tuple<Ts...>> { using type = std::variant<std::vector<Ts>...>; };
// Variant containing vectors of every component.
	using ComponentVectorVariant = typename VectorVariantGetter<Components>::type;

// Components of the same type are stored contiguously; one vector for each component.
	std::vector<ComponentVectorVariant>	m_components;
};
