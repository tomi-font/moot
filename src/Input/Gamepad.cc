#include <moot/Input/Gamepad.hh>
#include <moot/util/math/base.hh>
#include <algorithm>

constexpr float StickDeadZone = 0.2f;
constexpr float TriggerDeadZone = 0.05f;
constexpr float TriggerPressThreshold = 0.5f;

// 0 up to the dead zone, then rising from 0 to 1 at full travel.
static float beyondDeadZone(float value, float deadZone)
{
	if (value <= deadZone)
		return 0;
	return std::min((value - deadZone) / (1 - deadZone), 1.f);
}

bool Gamepad::isPressed(GamepadLayout::Button button) const
{
	return buttons[layout->getButtonIndex(button)];
}

bool Gamepad::isTriggerPressed(GamepadLayout::Trigger trigger) const
{
	if (const auto& button = layout->getTriggerButton(trigger))
		return buttons[*button];
	else
		return getTrigger(trigger) > TriggerPressThreshold;
}

float Gamepad::getTrigger(GamepadLayout::Trigger trigger) const
{
	const auto& axis = layout->getAxis(GamepadLayout::getTriggerAxis(trigger));
	if (!axis)
		return 0;
	// SFML reports a released trigger as -1 and a fully pulled one as 1.
	return beyondDeadZone((axes[unsigned(*axis)] + 1) / 2, TriggerDeadZone);
}

static float rawAxis(const decltype(Gamepad::axes)& axes, const std::optional<sf::Joystick::Axis>& axis)
{
	return axis ? axes[unsigned(*axis)] : 0;
}

Vector2f Gamepad::getStick(GamepadLayout::Stick stick) const
{
	// SFML counts a stick's y down the screen, the way pixels go.
	const Vector2f raw(rawAxis(axes, layout->getAxis(GamepadLayout::getXAxis(stick))),
	                   -rawAxis(axes, layout->getAxis(GamepadLayout::getYAxis(stick))));
	const float length = raw.length();
	if (length < StickDeadZone)
		return {};
	
	const float scaled = beyondDeadZone(length, StickDeadZone);
	return raw / length * scaled;
}

Vector2f Gamepad::getDPad() const
{
	// The d-pad comes through the hat axes, already digital: SFML reports -100, 0 or 100, with y down the screen like a stick.
	return {float(normalize(axes[unsigned(sf::Joystick::Axis::PovX)])),
	        -float(normalize(axes[unsigned(sf::Joystick::Axis::PovY)]))};
}

void Gamepad::resetInputs()
{
	buttons.reset();
	axes.fill(0);
	// A trigger rests at the far end of its travel.
	for (const GamepadLayout::Axis trigger : {GamepadLayout::Axis::LeftTrigger, GamepadLayout::Axis::RightTrigger})
		if (const auto& axis = layout->getAxis(trigger))
			axes[unsigned(*axis)] = -1;
}
