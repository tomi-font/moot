#pragma once

#include <Component/Group.hh>
#include <SFML/Graphics/RenderWindow.hpp>

class System
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
