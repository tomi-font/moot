#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <SFML/Window/Joystick.hpp>

// What a pad's raw button indices and SFML axis letters stand for.
class GamepadLayout
{
public:

	enum class Button
	{
		A,
		B,
		X,
		Y,
		LB,
		RB,
		Back,
		Start,
		LS,
		RS,
		COUNT
	};

	enum class Axis
	{
		LeftX,
		LeftY,
		RightX,
		RightY,
		LeftTrigger,
		RightTrigger,
		COUNT
	};

	enum class Stick
	{
		Left,
		Right
	};

	enum class Trigger
	{
		Left,
		Right
	};

	using ButtonIndices = std::array<unsigned, unsigned(Button::COUNT)>;
	using Axes = std::array<std::optional<sf::Joystick::Axis>, unsigned(Axis::COUNT)>;
	using TriggerButtons = std::array<std::optional<unsigned>, 2>;

	constexpr GamepadLayout(std::string_view name, const ButtonIndices& buttonIndices,
	                        const Axes& axes, const TriggerButtons& triggerButtons) :
		m_name(name),
		m_buttonIndices(buttonIndices),
		m_axes(axes),
		m_triggerButtons(triggerButtons)
	{}

	static const GamepadLayout* get(const sf::Joystick::Identification&, unsigned buttonCount);

	static constexpr Axis getXAxis(Stick stick) { return stick == Stick::Left ? Axis::LeftX : Axis::RightX; }
	static constexpr Axis getYAxis(Stick stick) { return stick == Stick::Left ? Axis::LeftY : Axis::RightY; }
	static constexpr Axis getTriggerAxis(Trigger trigger) { return trigger == Trigger::Left ? Axis::LeftTrigger : Axis::RightTrigger; }

	auto& name() const { return m_name; }
	unsigned getButtonIndex(Button button) const { return m_buttonIndices[unsigned(button)]; }
	// None when the pad has no such axis.
	auto& getAxis(Axis axis) const { return m_axes[unsigned(axis)]; }
	// The button a pad reports a trigger's press on as well, when it has one.
	auto& getTriggerButton(Trigger trigger) const { return m_triggerButtons[unsigned(trigger)]; }

private:

	std::string_view m_name;
	ButtonIndices m_buttonIndices;
	Axes m_axes;
	TriggerButtons m_triggerButtons;
};
