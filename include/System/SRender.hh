#pragma once

#include <System/System.hh>
#include <SFML/Graphics/Texture.hpp>

class SRender : public System
{
public:

	SRender();

private:

	void listenToEvents() override;
	void triggered(const Event&) override;

	void processInstantiatedEntity(const Entity&, unsigned) const override;
	void update(float elapsedTime) const override;
};
