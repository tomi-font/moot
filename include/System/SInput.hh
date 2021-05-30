#pragma once

#include <System/System.hh>
#include <Event/Listener.hh>

class SInput : public System, EventListener
{
public:

	SInput(EventManager&);

	void	update(sf::RenderWindow& window, float elapsedTime) override;
};
