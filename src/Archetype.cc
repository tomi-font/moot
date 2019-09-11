#include <Archetype.hh>
#include <Component/Component.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>

Archetype::Archetype(t_Comp c) : m_comp(c)
{
// prepares the archetype for storing all needed components

// we avoid using push_back because it would perform unnecessary copy
	if (c & C(Component::Position))
		m_cs.emplace_back(std::vector<CPosition>());
	if (c & C(Component::Render))
		m_cs.emplace_back(std::vector<CRender>());
}
