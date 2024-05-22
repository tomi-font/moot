#pragma once

#include <Component/Group.hh>
#include <Event/User.hh>
#include <Property/User.hh>

class Archetype;
class Window;

class System : public EventUser, public PropertyUser, sf::NonCopyable
{
public:

	virtual ~System();

	void setWindow(Window*);

	// Appends the archetype to matching component groups.
	void match(Archetype*);

	void processInstantiatedEntity(const Entity&) const;

	virtual void update(float elapsedTime) const = 0;

protected:

	// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;

	Window* m_window;

private:

	virtual void processInstantiatedEntity(const Entity&, unsigned) const;
};
