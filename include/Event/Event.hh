#pragma once

class Event
{
public:

	enum Type
	{
		// Keep the enum's values unassigned, so that it can be iterated.
		PlayerQuit,
		COUNT
	};

	Event(Type t) : m_type(t) {}

	Type type() const { return m_type; }

private:

	Type m_type;
};
