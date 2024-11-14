#pragma once

#include <moot/util/bit.hh>
#include <cassert>

// Interface type for construction of component IDs to prevent implicit conversions.
enum ComponentId : unsigned {};

// Describes a composition of components, i.e. which components are present.
class ComponentComposition
{
public:

	// Bitmask of all the composing components' IDs.
	using Bits = unsigned;

	constexpr ComponentComposition() : m_bits(0) {}
	constexpr ComponentComposition(ComponentId cid) : m_bits(1 << cid) {}
	constexpr ComponentComposition(Bits bits) : m_bits(bits) {}

	constexpr auto count() const { return setBitCount(m_bits); }
	constexpr bool empty() const { return !count(); }

	constexpr ComponentComposition operator+(ComponentComposition r) const { return m_bits | r.bits(); }
	constexpr void operator+=(ComponentComposition r)
	{
		assert(!has(r));
		m_bits |= r.bits();
	}
	constexpr void operator-=(ComponentComposition r)
	{
		assert(has(r));
		m_bits &= ~r.bits();
	}

	constexpr bool operator==(ComponentId cid) const { return ComponentId(*this) == cid; }
	constexpr bool operator==(ComponentComposition r) const { return m_bits == r.bits(); }

	constexpr bool has(ComponentId cid) const
	{
		return m_bits & ComponentComposition(cid).bits();
	}
	constexpr bool hasAllOf(ComponentComposition r) const { return (m_bits & r.bits()) == r.bits(); }
	constexpr bool hasNoneOf(ComponentComposition r) const { return !(m_bits & r.bits()); }

	constexpr Bits bits() const { return m_bits; }

	constexpr operator ComponentId() const
	{
		assert(count() == 1);
		return static_cast<ComponentId>(firstBitSetPos(m_bits));
	}

	class Iterator
	{
		Bits m_bits;
	public:
		Iterator(Bits bits) : m_bits(bits) {}
		bool operator==(const Iterator& right) const = default;
		void operator++() { m_bits ^= 1 << firstBitSetPos(m_bits); }
		ComponentId operator*() const { return static_cast<ComponentId>(firstBitSetPos(m_bits)); }
	};
	Iterator begin() const { return {m_bits}; }
	Iterator end() const { return {0}; }

private:

	Bits m_bits;
};
