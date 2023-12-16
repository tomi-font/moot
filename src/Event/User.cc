#include <Event/User.hh>
#include <Event/Manager.hh>
#include <cassert>

EventUser::EventUser(EventManager* em)
{
	setEventManager(em);
}

void EventUser::setEventManager(EventManager* em)
{
	assert(!m_eventManager);
	m_eventManager = em;
}

void EventUser::listen(Event::Type et)
{
	assert(m_eventManager);
	m_eventManager->addListener(et, this);
}

void EventUser::broadcast(const Event& e)
{
	assert(m_eventManager);
	m_eventManager->broadcast(e);
}
