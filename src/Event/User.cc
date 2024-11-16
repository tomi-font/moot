#include <moot/Event/User.hh>
#include <moot/Event/Manager.hh>
#include <cassert>

void EventUser::setEventManager(EventManager* em)
{
	assert(!m_eventManager && em);
	m_eventManager = em;
}

void EventUser::listenToEvents()
{
}

void EventUser::onEvent(const Event&)
{
}

void EventUser::listenTo(Event::Id eventId)
{
	assert(m_eventManager);
	m_eventManager->addListener(eventId, this);
}

void EventUser::trigger(const Event& e) const
{
	assert(m_eventManager);
	m_eventManager->trigger(e);
}
