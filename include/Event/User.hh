#pragma once

#include <Event/Manager.hh>

class EventListener
{
public:

	// Callback for when an event whose type was subscribed to is triggered.
	virtual void triggered(const Event&) {}

protected:

	EventListener(EventManager& em) : m_eventManager(em) {}

	// Start listening to the given type of event.
	void listen(Event::Type et) { m_eventManager.addListener(et, this); }

	void trigger(const Event& e) { m_eventManager.trigger(e); }

private:

	EventManager& m_eventManager;
};
