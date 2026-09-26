#include <moot/System/System.hh>

System::System() :
	m_lastUpdateChangeTick(),
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
	assert(m_schedule == SystemSchedule());
	m_schedule = schedule;
}

GlobalClock::Microseconds System::performUpdate()
{
	const GlobalClock::Microseconds thisUpdateStart = GlobalClock::microsecondsSinceStart();
	
	const GlobalChangeTick::Tick thisUpdateChangeTick = GlobalChangeTick::current();
	GlobalChangeTick::advance();

	update();

	m_lastUpdateChangeTick = thisUpdateChangeTick;

	return GlobalClock::microsecondsSinceStart() - thisUpdateStart;
}
