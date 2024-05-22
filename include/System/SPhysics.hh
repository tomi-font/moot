#pragma once

#include <System/System.hh>

class SPhysics : public System
{
public:

	SPhysics();

private:

	void processInstantiatedEntity(const Entity&, unsigned) const override;
	void update(float elapsedTime) const override;

	void moveEntity(const Entity&, const sf::Vector2f&) const;
};
