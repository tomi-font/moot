#pragma once

#include <moot/System/System.hh>

class SPhysics : public System
{
public:

	SPhysics();

private:

	void initializeProperties() override;

	void update() override;
};
