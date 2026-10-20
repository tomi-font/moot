#pragma once

#include <moot/System/System.hh>

class SDrive final : public System
{
public:

	SDrive();

private:

	void update() override;
};
