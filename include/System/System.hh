#pragma once

#include <Component/Group.hh>
#include <Event/User.hh>

class Archetype;
class Window;

class System : public EventUser
{
public:

	virtual ~System();

	void setWindow(Window*);

	// Appends the archetype to matching component groups.
	void match(Archetype*);

	bool initializes(ComponentComposition);

	virtual void initialize(const Entity&) const;
	virtual void update(float elapsedTime) const = 0;

protected:

	// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;

	Window* m_window;
};
