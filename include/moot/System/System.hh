#pragma once

#include <moot/Entity/Querier.hh>
#include <moot/Event/User.hh>
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

	virtual void update(float elapsedTime) const = 0;

protected:

	Window* m_window;
};
