#pragma once

#include <moot/Entity/Querier.hh>
#include <moot/Event/User.hh>
#include <moot/Global/ChangeTick.hh>
#include <moot/Global/Clock.hh>
#include <moot/Property/User.hh>
#include <moot/System/Schedule.hh>
#include <moot/TrackedValue.hh>

class EntityManager;
class Window;

class System :
	public EntityQuerier,
	public EventUser,
	public PropertyUser
{
public:

	System(const System&) = delete;
	System& operator=(const System&) = delete;

	~System() override;

	auto& schedule() const { return m_schedule; }

	void setEntityManager(EntityManager*);
	void setWindow(Window*);
	void setSchedule(SystemSchedule);

	// Returns how long the update took.
	GlobalClock::Microseconds performUpdate();

protected:

	System();

	template<typename T> bool hasChangedSinceLastUpdate(const TrackedValue<T>& tv)
	{
		return tv.hasChangedSince(m_lastUpdateChangeTick);
	}

	EntityManager* entityManager() const { return m_entityManager; }
	Window* window() const { return m_window; }

private:

	virtual void update() = 0;

	GlobalChangeTick::Tick m_lastUpdateChangeTick;

	EntityManager* m_entityManager;
	Window* m_window;

	SystemSchedule m_schedule;
};
