#include <Event/Listener.hh>

EventManager::EventManager() : m_listeners(Event::COUNT)
{
	// All event types are assumed to get used.																														
	for (unsigned et = 0; et != Event::COUNT; ++et)
	{
		m_listeners[static_cast<Event::Type>(et)];
	}
}

void	EventManager::addListener(Event::Type type, EventListener* listener)
{
	m_listeners.at(type).push_back(listener);
}

void	EventManager::trigger(const Event& event) const
{
	for (EventListener* listener : m_listeners.at(event.type()))
	{
		listener->triggered(event);
	}
}
