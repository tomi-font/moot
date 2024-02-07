#pragma once

#include <System/System.hh>

class SInput : public System
{
public:

	SInput();

	void update(float elapsedTime) const override;
};
