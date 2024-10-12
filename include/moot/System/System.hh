#pragma once

#include <moot/Entity/Query.hh>
#include <moot/Event/User.hh>
#include <moot/Property/User.hh>

class Archetype;
class Window;

class System : public EventUser, public PropertyUser, sf::NonCopyable
{
public:

	virtual ~System() override;

	void setWindow(Window*);

	void match(Archetype*);

	void initializeEntity(const Entity&) const;

	virtual void update(float elapsedTime) const = 0;

protected:

	System(unsigned queryCount);

	std::vector<EntityQuery> m_queries;

	Window* m_window;
};
