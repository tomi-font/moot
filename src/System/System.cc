#include <moot/System/System.hh>
#include <moot/Entity/Entity.hh>

System::System() :
	m_lastUpdateTicks(),
	m_window(nullptr)
{
}

System::~System()
{
}

void System::setWindow(Window* window)
{
	assert(!m_window && window);
	m_window = window;
}

void System::performUpdate()
{
	const GlobalClock::Ticks thisUpdateTicks = GlobalClock::ticksSinceStart();

	update();

	m_lastUpdateTicks = thisUpdateTicks;
}
