#pragma once

#include <moot/System/System.hh>

class SPhysics : public System
{
public:

	SPhysics();

private:

	void update(float elapsedTime) override;
};
