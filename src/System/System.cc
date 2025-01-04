#include <moot/System/System.hh>

System::System() :
	m_lastUpdateTicks(),
	m_entityManager(nullptr),
	m_window(nullptr)
{
}

System::~System()
{
}

void System::setEntityManager(EntityManager* entityManager)
{
	assert(!m_entityManager && entityManager);
	m_entityManager = entityManager;
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
