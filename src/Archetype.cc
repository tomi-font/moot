#include <Archetype.hh>
#include <Component/Component.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CRigidbody.hh>

Archetype::Archetype(CsComp comp) : m_comp(comp)
{
// prepares the archetype for storing all needed components

// one bit set in comp means one component
// thus, the number of set bits == number of vectors needed
	m_cs = new std::vector<void*>[__builtin_popcount(comp)];

// there should be some optimization here to avoid future reallocations
// using reserve() on all vectors depending on the composition
// __builtin_ctz() will be of use
}

Archetype::~Archetype()
{
	delete[] m_cs;
}
