#pragma once

#include <moot/System/System.hh>
#include <moot/Entity/Id.hh>
#include <moot/Input/State.hh>

class SInput final : public System
{
public:

	SInput();

private:

	void registerProperties() override;

	void start() override;

	void update() override;
	bool isKeyBound(sf::Keyboard::Key) const;
	void movePointer();
	void updatePointables();

	InputState m_state;
	EntityId m_pointedEntityId;
	bool m_logPadEvents;
};
