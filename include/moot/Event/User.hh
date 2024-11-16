#pragma once

#include <moot/Event/Event.hh>
#include <SFML/System/NonCopyable.hpp>

class EventManager;

class EventUser : sf::NonCopyable
{
public:

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
