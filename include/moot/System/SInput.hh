#pragma once

#include <moot/System/System.hh>

class SInput : public System
{
public:

	SInput();

private:

	void initializeProperties() override;

	void update(float elapsedTime) const override;
};
