#pragma once

#include <moot/System/System.hh>

class SLight final : public System
{
public:

	SLight();

private:

	void update() override;
};
