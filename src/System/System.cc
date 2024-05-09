#include <System/System.hh>
#include <Entity/Entity.hh>


System::~System()
{
}

void System::setWindow(Window* window)
{
	assert(!m_window && window);
	m_window = window;
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
