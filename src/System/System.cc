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

bool System::initializes(ComponentComposition comp)
{
	for (const ComponentGroup& group : m_groups)
		if (group.initializesEntities() && group.matches(comp))
			return true;
	return false;
}

void System::initialize(const Entity&) const
{
	assert(false);
}
