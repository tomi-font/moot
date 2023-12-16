#include <Event/Manager.hh>
#include <Event/User.hh>

EventManager::EventManager() : m_listeners(Event::COUNT)
{
	for (unsigned et = 0; et != Event::COUNT; ++et)
	{
		// Initializing the listeners map here allows const accesses via at() later on.
		m_listeners[static_cast<Event::Type>(et)];
	}
}

void EventManager::addListener(Event::Type type, EventUser* listener)
{
	m_listeners.at(type).push_back(listener);
}

void EventManager::broadcast(const Event& event) const
{
	for (EventUser* listener : m_listeners.at(event.type))
	{
		listener->triggered(event);
	}
}
