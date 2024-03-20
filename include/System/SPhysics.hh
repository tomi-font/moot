#pragma once

#include <System/System.hh>

class SPhysics : public System
{
public:

	SPhysics();

	void update(float elapsedTime) const override;

private:

	void moveEntity(const Entity&, const sf::Vector2f&) const;
};
