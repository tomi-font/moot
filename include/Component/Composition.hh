#pragma once

#include <cassert>

// Interface type for construction of component IDs to prevent implicit conversions.
enum ComponentIndex {};

// Describes a composition of components, i.e. which components are present.
class ComponentComposition
{
public:

	// Bitmask of all the composing components' IDs.
	using Bits = unsigned;

	constexpr ComponentComposition() : m_bits(0) {}
	constexpr ComponentComposition(ComponentIndex ci) : m_bits(1 << ci) {}
	constexpr ComponentComposition(Bits bits) : m_bits(bits) {}

	constexpr ComponentComposition operator|(ComponentComposition r) const { return m_bits | r.bits(); }
	constexpr void operator|=(ComponentComposition r) { m_bits |= r.bits(); }

	constexpr bool operator==(ComponentComposition r) const { return m_bits == r.bits(); }
	constexpr bool has(ComponentComposition r) const
	{
		assert(__builtin_popcount(r.bits()) == 1);
		return m_bits & r.bits();
	};
	constexpr bool hasAllOf(ComponentComposition r) const { return (m_bits & r.bits()) == r.bits(); }
	constexpr bool hasNoneOf(ComponentComposition r) const { return !(m_bits & r.bits()); }

	constexpr Bits bits() const { return m_bits; }

private:

	Bits m_bits;
};
