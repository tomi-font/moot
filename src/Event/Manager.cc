#include <moot/Event/Manager.hh>
#include <moot/Event/User.hh>

void EventManager::addListener(Event::Id eventId, EventUser* listener)
{
	m_listeners[eventId].push_back(listener);
}

void EventManager::trigger(const Event& event)
{
	for (EventUser* listener : m_listeners[event.id])
	{
		listener->eventTriggeredCallback(event);
	}
}
