#ifndef SINPUT_HH
#define SINPUT_HH

#include "System.hh"

class	SInput : public System
{
public:

	SInput();

// returns false if quitting was requested by the player
	bool	update(sf::RenderWindow& window, float elapsedTime) override;
};

#endif
