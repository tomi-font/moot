#include <System/System.hh>
#include <Archetype.hh>
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

Archetype* System::getEntitysArchetype(EntityId eid)
{
	const ComponentComposition comp = eid.comp();

	for (ComponentGroup& group : m_groups)
	{
		for (Archetype* arch : group.archs())
		{
			if (arch->comp() == comp)
				return arch;
		}
	}
	return nullptr;
}
