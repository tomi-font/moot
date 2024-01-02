#pragma once

#include <System/System.hh>

class SPhysics : public System
{
public:

	SPhysics();

	void update(float elapsedTime) override;
};
