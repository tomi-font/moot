#pragma once

#include <moot/System/System.hh>
#include <SFML/Graphics/Texture.hpp>

class SRender : public System
{
public:

	SRender();

private:

	void listenToEvents() override;
	void triggered(const Event&) override;

	void update(float elapsedTime) const override;
};
