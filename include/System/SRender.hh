#pragma once

#include <System/System.hh>
#include <SFML/Graphics/Texture.hpp>

class SRender : public System
{
public:

	SRender();

	void listenToEvents() override;
	void triggered(const Event&) override;

	void update(float elapsedTime) const override;
};
