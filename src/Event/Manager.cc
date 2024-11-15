#include <moot/Event/Manager.hh>
#include <moot/Event/User.hh>

void EventManager::addListener(Event::Id eventId, EventUser* listener)
{
	m_listeners[eventId].push_back(listener);
}

void EventManager::trigger(const Event& event) const
{
	if (const auto it = m_listeners.find(event.id); it != m_listeners.end())
		for (EventUser* listener : it->second)
			listener->eventTriggeredCallback(event);
}
