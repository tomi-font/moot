#include <moot/Input/State.hh>
#include <algorithm>
#include <cassert>
#include <cmath>

InputState::InputState() :
	m_pointerIsInWindow(false),
	m_maxFrameStickTravel(0),
	m_active(false)
{
	for (Gamepad& pad : m_pads)
	{
		pad.joystickId = unsigned(&pad - m_pads.begin());
		pad.connected = false;
		pad.layout = GamepadLayout::get({}, 0);
		pad.resetInputs();
	}
}

void InputState::beginFrame(float maxFrameStickTravel)
{
	m_mouseMotion = {};
	m_maxFrameStickTravel = maxFrameStickTravel;
}

void InputState::apply(const sf::Event& event)
{
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code != sf::Keyboard::Key::Unknown)
			m_keys.set(unsigned(keyPressed->code));
	}
	else if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>())
	{
		if (keyReleased->code != sf::Keyboard::Key::Unknown)
			m_keys.reset(unsigned(keyReleased->code));
	}
	else if (const auto* buttonPressed = event.getIf<sf::Event::MouseButtonPressed>())
	{
		m_mouseButtons.set(unsigned(buttonPressed->button));
	}
	else if (const auto* buttonReleased = event.getIf<sf::Event::MouseButtonReleased>())
	{
		m_mouseButtons.reset(unsigned(buttonReleased->button));
	}
	else if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>())
	{
		m_pointer = mouseMoved->position;
	}
	else if (event.is<sf::Event::MouseEntered>())
	{
		m_pointerIsInWindow = true;
	}
	else if (event.is<sf::Event::MouseLeft>())
	{
		m_pointerIsInWindow = false;
	}
	else if (const auto* mouseMovedRaw = event.getIf<sf::Event::MouseMovedRaw>())
	{
		if (m_active)
			m_mouseMotion += Vector2f(mouseMovedRaw->delta.x, -mouseMovedRaw->delta.y);
	}
	else if (event.is<sf::Event::FocusLost>())
	{
		m_active = false;
		m_keys.reset();
		m_mouseButtons.reset();
		for (Gamepad& pad : m_pads)
			if (pad.connected)
				pad.resetInputs();
	}
	else if (event.is<sf::Event::FocusGained>())
	{
		m_active = true;
		for (const Gamepad& pad : m_pads)
			if (pad.connected)
				readPad(pad.joystickId);
	}
	else if (const auto* connected = event.getIf<sf::Event::JoystickConnected>())
	{
		connectPad(connected->joystickId);
		if (m_active)
			readPad(connected->joystickId);
	}
	else if (const auto* disconnected = event.getIf<sf::Event::JoystickDisconnected>())
	{
		Gamepad& pad = m_pads[disconnected->joystickId];
		pad.connected = false;
		pad.resetInputs();
	}
	else if (const auto* padButtonPressed = event.getIf<sf::Event::JoystickButtonPressed>())
	{
		if (m_active)
			m_pads[padButtonPressed->joystickId].buttons.set(padButtonPressed->button);
	}
	else if (const auto* padButtonReleased = event.getIf<sf::Event::JoystickButtonReleased>())
	{
		if (m_active)
			m_pads[padButtonReleased->joystickId].buttons.reset(padButtonReleased->button);
	}
	else if (const auto* padMoved = event.getIf<sf::Event::JoystickMoved>())
	{
		if (m_active)
			m_pads[padMoved->joystickId].axes[unsigned(padMoved->axis)] = padMoved->position / 100.f;
	}
}

void InputState::connectPad(unsigned joystickId)
{
	Gamepad& pad = m_pads[joystickId];

	pad.connected = true;
	pad.layout = GamepadLayout::get(sf::Joystick::getIdentification(joystickId), sf::Joystick::getButtonCount(joystickId));
}

void InputState::readPad(unsigned joystickId)
{
	Gamepad& pad = m_pads[joystickId];

	for (unsigned button = sf::Joystick::getButtonCount(joystickId); button--;)
		pad.buttons[button] = sf::Joystick::isButtonPressed(joystickId, button);

	for (unsigned axis = 0; axis != sf::Joystick::AxisCount; ++axis)
		if (sf::Joystick::hasAxis(joystickId, sf::Joystick::Axis(axis)))
			pad.axes[axis] = sf::Joystick::getAxisPosition(joystickId, sf::Joystick::Axis(axis)) / 100.f;
}

bool InputState::isPressed(sf::Keyboard::Key key) const
{
	return m_keys[unsigned(key)];
}

bool InputState::isPressed(sf::Mouse::Button button) const
{
	return m_mouseButtons[unsigned(button)];
}

const Gamepad& InputState::getPad() const
{
	for (const Gamepad& pad : m_pads)
		if (pad.connected)
			return pad;
	return m_pads[0];
}

Vector2i InputState::getPointerPosition() const
{
	return {int(std::lround(m_pointer.x)), int(std::lround(m_pointer.y))};
}

void InputState::setPointer(const Vector2i& pointer, bool isInWindow)
{
	m_pointer = Vector2f(pointer);
	m_pointerIsInWindow = isInWindow;
}

void InputState::movePointer(const Vector2f& delta, const Vector2u& windowSize)
{
	m_pointer += delta;
	m_pointer.x = std::clamp(m_pointer.x, 0.f, windowSize.x - 1.f);
	m_pointer.y = std::clamp(m_pointer.y, 0.f, windowSize.y - 1.f);
	m_pointerIsInWindow = true;
}
