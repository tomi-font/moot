#include <Archetype.hh>
#include <Component/Component.hh>
#include <Component/CTransform.hh>
#include <Component/CRender.hh>

Archetype::Archetype(t_Comp c) : m_comp(c)
{
// prepares the archetype for storing all needed components

// we avoid using push_back because it would perform unnecessary copy
	if (c & C(Component::Transform))
		m_cs.emplace_back(std::vector<CTransform>());
	if (c & C(Component::Render))
		m_cs.emplace_back(std::vector<CRender>());
}
