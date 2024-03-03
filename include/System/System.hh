#pragma once

#include <Component/Group.hh>
#include <Event/User.hh>

class Archetype;
namespace sf { class RenderWindow; }

class System : public EventUser
{
public:

	System();
	virtual ~System();

	virtual void update(float elapsedTime) const = 0;

	// Appends the archetype to matching component groups.
	void match(Archetype*);

protected:

	// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;

	sf::RenderWindow* const m_window;

private:

	Archetype* getArchetype(ComponentComposition) const;	
};
