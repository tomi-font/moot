#pragma once

#include "System.hh"

class	SPhysics : public System
{
public:

	SPhysics();

	void	update(sf::RenderWindow& window, float elapsedTime) override;
};
