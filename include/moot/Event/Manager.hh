#pragma once

#include <moot/Event/Event.hh>
#include <unordered_map>
#include <vector>

class EventUser;

class EventManager
{
public:

	void addListener(Event::Id, EventUser*);

	void trigger(const Event&);

private:

	std::unordered_map<Event::Id, std::vector<EventUser*>> m_listeners;
};
