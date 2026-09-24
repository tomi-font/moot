#pragma once

#include <moot/Event/Event.hh>

class EventManager;

class EventUser
{
public:

	EventUser() = default;
	virtual ~EventUser() = default;

	EventUser(const EventUser&) = delete;
	EventUser& operator=(const EventUser&) = delete;

	void initializeEvents(EventManager*);

	virtual void onEvent(const Event&);

protected:

	void listenTo(Event::Id);

	void trigger(const Event&) const;

private:

	virtual void listenToEvents();

	EventManager* m_eventManager = nullptr;
};
