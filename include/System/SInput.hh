#pragma once

#include <System/System.hh>

class SInput : public System
{
public:

	SInput();

	void update(sf::RenderWindow& window, float elapsedTime) override;
};
