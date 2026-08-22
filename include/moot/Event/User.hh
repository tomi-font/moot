#pragma once

#include <moot/Event/Event.hh>

class EventManager;

class EventUser
{
public:

	EventUser() = default;
	EventUser(const EventUser&) = delete;
	EventUser& operator=(const EventUser&) = delete;

	void setEventManager(EventManager*);
	virtual void listenToEvents();

	virtual void onEvent(const Event&);

protected:

	virtual ~EventUser() {}

	void listenTo(Event::Id);

	void trigger(const Event&) const;

private:

	EventManager* m_eventManager = nullptr;
};
