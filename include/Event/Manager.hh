#ifndef EVENT_MANAGER_HH
#define EVENT_MANAGER_HH

#include "Event.hh"
#include <unordered_map>
#include <vector>

class EventListener;

class EventManager
{
public:

	EventManager();

	void addListener(Event::Type, EventListener*);

	void trigger(const Event&) const;

private:

	std::unordered_map<Event::Type, std::vector<EventListener*>> m_listeners;
};

#endif
