#pragma once

#include <moot/Entity/Querier.hh>
#include <moot/Event/User.hh>
#include <moot/Global/Clock.hh>
#include <moot/Property/User.hh>

class Archetype;
class Window;

class System :
	public EntityQuerier,
	public EventUser,
	public PropertyUser,
	sf::NonCopyable
{
public:

	virtual ~System() override;

	void setWindow(Window*);

	void performUpdate();

protected:

	System();

	template<typename T> constexpr bool
	hasChangedSinceLastUpdate(const TrackedValue<T>& tv) { return tv.hasChangedSince(m_lastUpdateTicks); }

	Window* window() const { return m_window; }

private:

	GlobalClock::Ticks m_lastUpdateTicks;
	Window* m_window;

	virtual void update() = 0;
};
