#pragma once

#include <moot/Entity/Querier.hh>
#include <moot/Event/User.hh>
#include <moot/Global/Clock.hh>
#include <moot/Property/User.hh>
#include <moot/TrackedValue.hh>

class EntityManager;
class Window;

class System :
	public EntityQuerier,
	public EventUser,
	public PropertyUser,
	sf::NonCopyable
{
public:

	virtual ~System() override;

	void setEntityManager(EntityManager*);
	void setWindow(Window*);

	void performUpdate();

protected:

	System();

	template<typename T> inline bool hasChangedSinceLastUpdate(const TrackedValue<T>& tv)
	{
		return tv.hasChangedSince(m_lastUpdateTicks);
	}

	EntityManager* entityManager() const { return m_entityManager; }
	Window* window() const { return m_window; }

private:

	virtual void update() = 0;

	GlobalClock::Ticks m_lastUpdateTicks;

	EntityManager* m_entityManager;
	Window* m_window;
};
