#pragma once

class Event
{
public:

	enum Type
	{
		// Don't assign the enum values, so that they're iterable.
		PlayerQuit,
		COUNT // Keep last.
	};

	Event(Type t) : m_type(t) {}

	Type type() const { return m_type; }

private:

	Type m_type;
};
