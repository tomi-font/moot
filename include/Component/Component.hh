#pragma once

struct Component
{
	enum Type
	{
	};
};

// Describes a composition of components.
class ComponentComposition
{
public:

// Bitmask of all the composing components' ids.
	using Bits = unsigned;

	constexpr ComponentComposition() : m_bits(0) {}
	constexpr ComponentComposition(Component::Type ct) : m_bits(1 << ct) {}

	constexpr ComponentComposition	operator|(ComponentComposition r) const { return m_bits | r.m_bits; }
	constexpr ComponentComposition	operator&(ComponentComposition r) const { return m_bits & r.m_bits; }
	constexpr bool	operator==(ComponentComposition r) const { return m_bits == r.m_bits; }

	constexpr Bits	bits() const { return m_bits; }

private:

	constexpr ComponentComposition(Bits bits) : m_bits(bits) {}

	Bits	m_bits;
};
