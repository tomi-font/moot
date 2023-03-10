#pragma once

#include <Event/Event.hh>
#include <unordered_map>
#include <vector>

class EventListener;

class EventManager
{
public:

	EventManager();

	// Subscribes a listener to an event type.
	void addListener(Event::Type, EventListener*);

	// Broadcasts an event to the subscribed listeners.
	void trigger(const Event&) const;

private:

	// Maps the event types to their subscribed listeners.
	std::unordered_map<Event::Type, std::vector<EventListener*>> m_listeners;
};
