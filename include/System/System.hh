#pragma once

#include <Component/Group.hh>
#include <Entity/Id.hh>
#include <Event/User.hh>

class Archetype;
namespace sf { class RenderWindow; }

class System : public EventUser
{
public:

	virtual ~System() {}

	virtual void update(sf::RenderWindow& window, float elapsedTime) = 0;

	// Appends the archetype to matching component groups.
	void match(Archetype*);

protected:

	Archetype* getEntitysArchetype(EntityId);

	// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;
};
