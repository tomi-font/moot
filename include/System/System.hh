#ifndef SYSTEM_HH
#define SYSTEM_HH

#include "../Component/Group.hh"
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>

class	System
{
public:

	virtual ~System(){}

// The various sytems, declared in update order.
	enum
	{
		Input,
		Physics,
		Render,
		COUNT // Keep last.
	};

	virtual bool update(sf::RenderWindow& window, float elapsedTime) = 0;

// Appends the archetype to matching component groups.
	void match(Archetype*);

protected:

// Component groups of interest.
	std::vector<ComponentGroup>	m_groups;
};

#endif
