#pragma once

#include <Component/Group.hh>
#include <Event/User.hh>

namespace sf { class RenderWindow; }

class System : public EventUser
{
public:

	virtual ~System() {}

	virtual void update(sf::RenderWindow& window, float elapsedTime) = 0;

	// Appends the archetype to matching component groups.
	void match(Archetype*);

protected:

	// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;
};
