#include <Event/Listener.hh>

EventManager::EventManager() : m_listeners(Event::COUNT)
{
	for (unsigned et = 0; et != Event::COUNT; ++et)
	{
		// Initializing the listeners map here allows const accesses via at() later on.
		m_listeners[static_cast<Event::Type>(et)];
	}
}

void EventManager::addListener(Event::Type type, EventListener* listener)
{
	m_listeners.at(type).push_back(listener);
}

void EventManager::trigger(const Event& event) const
{
	for (EventListener* listener : m_listeners.at(event.type()))
	{
		listener->triggered(event);
	}
}
