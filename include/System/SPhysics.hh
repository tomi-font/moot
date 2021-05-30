#pragma once

#include <System/System.hh>

class SPhysics : public System
{
public:

	SPhysics();

	void	update(sf::RenderWindow& window, float elapsedTime) override;
};
