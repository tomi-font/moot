#pragma once

#include <moot/System/System.hh>

// Keeps every child entity at its offset from its parent.
class SHierarchy final : public System
{
public:

	SHierarchy();

private:

	void update() override;
};
