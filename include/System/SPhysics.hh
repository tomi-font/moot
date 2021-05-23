#ifndef SPHYSICS_HH
#define SPHYSICS_HH

#include "System.hh"

class	SPhysics : public System
{
public:

	SPhysics();

	bool	update(sf::RenderWindow& window, float elapsedTime) override;
};

#endif
