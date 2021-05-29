#pragma once

#include "Manager.hh"

class EventListener
{
public:

	virtual void triggered(const Event&) {}

protected:

	EventListener(EventManager& em) : m_eventManager(em) {}

	void listen(Event::Type et) { m_eventManager.addListener(et, this); }

	void trigger(const Event& e) { m_eventManager.trigger(e); }

private:

	EventManager& m_eventManager;
};
