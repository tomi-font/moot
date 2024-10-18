#include <moot/System/System.hh>
#include <moot/Entity/Entity.hh>

System::~System()
{
}

void System::setWindow(Window* window)
{
	assert(!m_window && window);
	m_window = window;
}
