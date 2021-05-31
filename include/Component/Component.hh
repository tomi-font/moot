#pragma once

class Component
{
public:

	enum Type
	{
		Position,
		Render,
		Move,
		Player,
		CollisionBox,
		Rigidbody,
		COUNT // Keep last.
	};
};

// Component composition; describes which components are present.
class CsComp
{
public:

// Bitmask of all the composing Component::Type.
	using Bits = unsigned;

	constexpr CsComp() : m_bits(0) {}
	constexpr CsComp(Component::Type ct) : m_bits(1 << ct) {}

	constexpr CsComp	operator|(CsComp r) const {	return m_bits | r.m_bits; }
	constexpr CsComp	operator&(CsComp r) const { return m_bits & r.m_bits; }
	constexpr bool		operator==(CsComp r) const { return m_bits == r.m_bits; }

	constexpr Bits	bits() const { return m_bits; }

private:

	constexpr CsComp(Bits bits) : m_bits(bits) {}

	Bits	m_bits;
};

constexpr CsComp operator|(Component::Type l, Component::Type r) { return CsComp(l) | r; }
