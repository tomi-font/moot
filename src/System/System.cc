#include <System/System.hh>
#include <cassert>

void System::setWindow(sf::RenderWindow* window)
{
	assert(!m_window);
	m_window = window;
}

void System::match(Archetype* arch)
{
	for (ComponentGroup& group : m_groups)
		group.match(arch);
}

EntityHandle System::getEntity(EntityId eid) const
{
	return { getArchetype(eid.comp), eid.index };
}

Archetype* System::getArchetype(ComponentComposition comp) const
{
	for (const ComponentGroup& group : m_groups)
	{
		if (group.matches(comp))
			return group.getArchetype(comp);
	}
	return nullptr;
}
