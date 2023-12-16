#pragma once

#include <Event/Event.hh>
#include <unordered_map>
#include <vector>

class EventUser;

class EventManager
{
public:

	EventManager();

	// Subscribes a listener to an event type.
	void addListener(Event::Type, EventUser*);

	// Broadcasts an event to the subscribed listeners.
	void broadcast(const Event&) const;

private:

	// Maps the event types to their subscribed listeners.
	std::unordered_map<Event::Type, std::vector<EventUser*>> m_listeners;
};
