#include <System/System.hh>
#include <utility/Window.hh>
#include <cassert>

System::System() : m_window(Window::window)
{
	assert(m_window);
}

System::~System()
{
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
