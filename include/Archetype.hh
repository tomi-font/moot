#pragma once

#include <Component/Component.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CRigidbody.hh>
#include <vector>
#include <variant>

// Archetypes store components of entities that are the same,
// i.e. possess exactly the same components

class Archetype
{
public:

	Archetype(CsComp);

	Archetype(const Archetype&) = delete;
	void	operator=(const Archetype&) = delete;

	CsComp	comp() const noexcept { return m_comp; }

// returns the vector containing the component type requested
// behavior is undefined if the caller asks for some component that's not present
	template<typename T>
	std::vector<T>&	get() noexcept
	{
		// we calculate the index in m_components by finding out how many bits are set up to
		// C(T::Type), as components will always be stored in the ComponentVariant order
		return std::get<std::vector<T>>(m_components[__builtin_popcount(m_comp.bits() & (CsComp(T::Type).bits() - 1))]);
	}

private:

// components composition for this archetype
	CsComp	m_comp;

// components of the same type are stored contiguously ; one vector for each component
// has to remain in the same order than Component::Type for being able to index from it
	using ComponentVariant = std::variant<
		std::vector<CPosition>,
		std::vector<CRender>,
		std::vector<CMove>,
		std::vector<CPlayer>,
		std::vector<CCollisionBox>,
		std::vector<CRigidbody>>;

	std::vector<ComponentVariant>	m_components;
};
