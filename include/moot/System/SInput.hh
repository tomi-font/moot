#pragma once

#include <moot/System/System.hh>

class SInput final : public System
{
public:

	SInput();

private:

	void initializeProperties() override;

	void update() override;
	void updatePointables();

	std::optional<sf::Vector2i> m_mousePos;
	EntityId m_pointedEntityId;
};
