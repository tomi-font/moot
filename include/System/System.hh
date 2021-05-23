#ifndef SYSTEM_HH
#define SYSTEM_HH

#include "../Component/Group.hh"
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>

class	System
{
public:

// The various sytems, declared in update order.
	enum
	{
		Input,
		Physics,
		Render,
		COUNT // Keep last.
	};

	virtual bool update(sf::RenderWindow& window, float elapsedTime) = 0;

	std::vector<ComponentGroup>& getGroups() noexcept
	{
		return m_groups;
	}

protected:

	std::vector<ComponentGroup>	m_groups;
};

#endif
