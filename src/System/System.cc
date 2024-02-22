#include <System/System.hh>
#include <cassert>

System::~System()
{
}

void System::initWindow(sf::RenderWindow* window)
{
	assert(!m_window);
	m_window = window;
}

void System::match(Archetype* arch)
{
	for (ComponentGroup& group : m_groups)
		group.match(arch);
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
