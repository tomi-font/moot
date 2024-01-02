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

	virtual void update(float elapsedTime) = 0;

	void setWindow(sf::RenderWindow*);

	// Appends the archetype to matching component groups.
	void match(Archetype*);

protected:

	Archetype* getEntitysArchetype(EntityId);

	// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;

	sf::RenderWindow* m_window = nullptr;
};
