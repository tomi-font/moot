#pragma once

#include <moot/System/System.hh>

class SPhysics final : public System
{
public:

	SPhysics();

private:

	void initializeProperties() override;

	void update() override;
};
