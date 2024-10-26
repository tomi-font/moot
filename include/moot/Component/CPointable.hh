#pragma once

#include <functional>

class Entity;

class CPointable
{
	using Callback = std::function<void(const Entity&)>;

public:

	enum EventType
	{
		PointerEntered,
		PointerLeft
	};
	void setCallback(EventType et, Callback&& cb) { m_callbacks[et] = std::move(cb); }
	void notify(EventType, const Entity&) const;

private:

	std::unordered_map<EventType, Callback> m_callbacks;
};
