#pragma once

#include <functional>

struct EntityHandle;

class CPointable
{
	using Callback = std::function<void(EntityHandle&)>;

public:

	enum EventType
	{
		PointerEntered,
		PointerLeft
	};
	void setCallback(EventType et, Callback&& cb) { m_callbacks[et] = std::move(cb); }
	void notify(EventType, EntityHandle&) const;

private:

	std::unordered_map<EventType, Callback> m_callbacks;
};
