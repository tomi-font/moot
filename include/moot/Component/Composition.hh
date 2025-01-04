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
	constexpr ComponentComposition(ComponentId cId) : m_bits(1 << cId) {}
	constexpr ComponentComposition(Bits bits) : m_bits(bits) {}

	constexpr auto count() const { return setBitCount(m_bits); }
	constexpr bool empty() const { return !m_bits; }

	constexpr auto& operator+=(ComponentComposition r)
	{
		assert(hasNoneOf(r));
		m_bits |= r.m_bits;
		return *this;
	}
	constexpr auto& operator-=(ComponentComposition r)
	{
		assert(hasAllOf(r));
		m_bits &= ~r.m_bits;
		return *this;
	}
	constexpr auto operator+(ComponentComposition r) const
	{
		return ComponentComposition(m_bits) += r;
	}
	constexpr auto operator-(ComponentComposition r) const
	{
		return ComponentComposition(m_bits) -= r;
	}

	constexpr bool operator==(ComponentId cId) const { return ComponentId(*this) == cId; }
	constexpr bool operator==(ComponentComposition r) const { return m_bits == r.m_bits; }

	constexpr bool has(ComponentId cId) const
	{
		return m_bits & ComponentComposition(cId).m_bits;
	}
	constexpr bool hasAllOf(ComponentComposition r) const { return (m_bits & r.m_bits) == r.m_bits; }
	constexpr bool hasNoneOf(ComponentComposition r) const { return !(m_bits & r.m_bits); }

	constexpr Bits bits() const { return m_bits; }

	constexpr operator ComponentId() const
	{
		assert(count() == 1);
		return static_cast<ComponentId>(firstBitSetPos(m_bits));
	}

	constexpr unsigned indexOf(ComponentId cId) const
	{
		assert(has(cId));
		return setBitCount(m_bits & (ComponentComposition(cId).m_bits - 1));
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
