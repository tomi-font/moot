#pragma once

#include <moot/System/System.hh>

class SHierarchy final : public System
{
public:

	SHierarchy();

private:

	void update() override;
	void placeChildren(const EntityPointer& parent);
};
