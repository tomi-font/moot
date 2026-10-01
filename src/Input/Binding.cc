#include <moot/Input/Binding.hh>
#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>

static bool fits(const Control& control, Binding::Type type)
{
	using Type = Binding::Type;

	if (type == Type::Button)
		return control.isDigital() || control.isTrigger();
	if (type == Type::Axis)
		return control.isAxis();
	if (type == Type::Axes)
		return control.isAxes();
	if (type == Type::Motion)
		return control.isMotion();
	if (type == Type::Scroll)
		return control.isScroll();

	std::unreachable();
}

static Binding::Value initialValue(Binding::Type type)
{
	using Type = Binding::Type;

	if (type == Type::Button)
		return false;
	if (type == Type::Axis || type == Type::Scroll)
		return 0.f;
	if (type == Type::Axes || type == Type::Motion)
		return Vector2f();
	if (type == Type::Pointer)
		return Vector2i();

	std::unreachable();
}

Binding::Binding(Type type, std::vector<Control>&& controls) :
	m_type(type),
	m_controls(std::move(controls)),
	m_lastValue(initialValue(type))
{
	assert(m_controls.empty() == (type == Type::Pointer));

	for (const Control& control : m_controls)
		assert(fits(control, type));
}

bool Binding::hasKey(sf::Keyboard::Key key) const
{
	for (const Control& control : m_controls)
		if (control.hasKey(key))
			return true;
	return false;
}

bool Binding::isAnyPressed(const InputState& state) const
{
	for (const Control& control : m_controls)
		if (control.isPressed(state))
			return true;
	return false;
}

void Binding::deliver(const Value& value, EntityHandle& entity)
{
	m_callback(entity, value);
	m_lastValue = value;
}

void Binding::deliverIfChanged(const Value& value, EntityHandle& entity)
{
	if (value != m_lastValue)
		deliver(value, entity);
}

bool Binding::deliversOn(const sf::Event& event)
{
	return event.is<sf::Event::KeyPressed>() || event.is<sf::Event::KeyReleased>()
	    || event.is<sf::Event::MouseButtonPressed>() || event.is<sf::Event::MouseButtonReleased>()
	    || event.is<sf::Event::JoystickButtonPressed>() || event.is<sf::Event::JoystickButtonReleased>()
	    || event.is<sf::Event::MouseWheelScrolled>();
}

void Binding::deliverEventUpdate(const sf::Event& event, const InputState& state, EntityHandle& entity)
{
	if (m_type == Type::Button)
	{
		deliverIfChanged(isAnyPressed(state), entity);
	}
	else if (m_type == Type::Scroll)
	{
		float notches = 0;
		for (const Control& control : m_controls)
			notches += control.getScroll(event, state);
		if (notches != 0)
			deliver(notches, entity);
	}
}

void Binding::deliverFrameUpdate(const InputState& state, EntityHandle& entity)
{
	if (m_type == Type::Button)
	{
		// What no press or release announced: the input going inactive, a pad gone, a trigger standing for a button.
		deliverIfChanged(isAnyPressed(state), entity);
	}
	else if (m_type == Type::Axis)
	{
		float value = 0;
		for (const Control& control : m_controls)
			value += control.getAxis(state);
		deliverIfChanged(std::clamp(value, -1.f, 1.f), entity);
	}
	else if (m_type == Type::Axes)
	{
		Vector2f value;
		for (const Control& control : m_controls)
			value += control.getAxes(state);

		// Kept inside the unit circle so that a keyboard diagonal is no faster than a stick's corner.
		if (const float length = value.length(); length > 1)
		{
			value = value / length;
			if (value.length() > 1) // Rounding can leave it a hair over 1.
				value *= 1 - std::numeric_limits<float>::epsilon();
			assert(value.length() <= 1);
		}
		deliverIfChanged(value, entity);
	}
	else if (m_type == Type::Pointer)
	{
		deliverIfChanged(state.getPointerPosition(), entity);
	}
	else if (m_type == Type::Motion)
	{
		Vector2f motion;
		for (const Control& control : m_controls)
			motion += control.getMotion(state);
		if (motion.isNotZero())
			deliver(motion, entity);
	}
}
