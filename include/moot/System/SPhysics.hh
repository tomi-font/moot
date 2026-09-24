#pragma once

#include <moot/System/System.hh>

class SPhysics final : public System
{
public:

	SPhysics();

private:

	void registerProperties() override;

	void update() override;
};
