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

void System::gameLoopUpdate(float elapsedTime)
{
	const GlobalClock::Ticks thisUpdateTicks = GlobalClock::ticksSinceStart();
	update(elapsedTime);
	m_lastUpdateTicks = thisUpdateTicks;
}
