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

void System::setSchedule(SystemSchedule schedule)
{
	assert(m_schedule.phase == SystemSchedule().phase
	    && m_schedule.order.before == SystemSchedule().order.before);
	m_schedule = schedule;
}

GlobalClock::Ticks System::performUpdate()
{
	const GlobalClock::Ticks thisUpdateTicks = GlobalClock::ticksSinceStart();

	update();

	m_lastUpdateTicks = thisUpdateTicks;

	return GlobalClock::ticksSinceStart() - thisUpdateTicks;
}
