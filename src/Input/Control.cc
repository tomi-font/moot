#include <moot/Input/Control.hh>
#include <cassert>
#include <utility>

template<typename T> static bool is(const Control::Physical& physical)
{
	return std::holds_alternative<T>(physical);
}

bool Control::isDigital() const
{
	return is<Key>(m_physical) || is<MouseButton>(m_physical) || is<PadButton>(m_physical);
}

bool Control::isTrigger() const
{
	return is<PadTrigger>(m_physical);
}

bool Control::isAxis() const
{
	return is<ButtonPair>(m_physical) || is<PadTrigger>(m_physical) || is<PadStickAxis>(m_physical) || is<PadDPadAxis>(m_physical);
}

bool Control::isAxes() const
{
	return is<ButtonQuad>(m_physical) || is<PadStick>(m_physical) || is<PadDPad>(m_physical);
}

bool Control::isMotion() const
{
	return is<MouseMotion>(m_physical) || is<PadStick>(m_physical);
}

bool Control::isScroll() const
{
	return is<MouseWheel>(m_physical) || is<ButtonPair>(m_physical);
}

Control::Digital Control::asDigital() const
{
	assert(isDigital());

	if (const auto* key = std::get_if<Key>(&m_physical))
		return *key;
	if (const auto* mouseButton = std::get_if<MouseButton>(&m_physical))
		return *mouseButton;
	if (const auto* padButton = std::get_if<PadButton>(&m_physical))
		return *padButton;

	std::unreachable();
}

static bool isKey(const Control::Digital& digital, sf::Keyboard::Key key)
{
	const auto* const digitalKey = std::get_if<Control::Key>(&digital);
	return digitalKey && digitalKey->code == key;
}

bool Control::hasKey(sf::Keyboard::Key key) const
{
	if (const auto* ownKey = std::get_if<Key>(&m_physical))
		return ownKey->code == key;
	if (const auto* pair = std::get_if<ButtonPair>(&m_physical))
		return isKey(pair->negative, key) || isKey(pair->positive, key);
	if (const auto* quad = std::get_if<ButtonQuad>(&m_physical))
		return isKey(quad->x.negative, key) || isKey(quad->x.positive, key)
		    || isKey(quad->y.negative, key) || isKey(quad->y.positive, key);
	return false;
}

static bool isDigitalPressed(const Control::Digital& digital, const InputState& state)
{
	if (const auto* key = std::get_if<Control::Key>(&digital))
		return state.isPressed(key->code);
	if (const auto* mouseButton = std::get_if<Control::MouseButton>(&digital))
		return state.isPressed(mouseButton->button);
	return state.getPad().isPressed(std::get<Control::PadButton>(digital).button);
}

bool Control::isPressed(const InputState& state) const
{
	if (const auto* trigger = std::get_if<PadTrigger>(&m_physical))
		return state.getPad().isTriggerPressed(trigger->trigger);
	return isDigitalPressed(asDigital(), state);
}

static float getPairAxis(const Control::ButtonPair& pair, const InputState& state)
{
	return float(isDigitalPressed(pair.positive, state)) - float(isDigitalPressed(pair.negative, state));
}

float Control::getAxis(const InputState& state) const
{
	assert(isAxis());

	if (const auto* pair = std::get_if<ButtonPair>(&m_physical))
		return getPairAxis(*pair, state);
	if (const auto* trigger = std::get_if<PadTrigger>(&m_physical))
		return state.getPad().getTrigger(trigger->trigger);
	if (const auto* stickAxis = std::get_if<PadStickAxis>(&m_physical))
	{
		const Vector2f stick = state.getPad().getStick(stickAxis->stick);
		return stickAxis->vertical ? stick.y : stick.x;
	}
	const Vector2f dPad = state.getPad().getDPad();
	return std::get<PadDPadAxis>(m_physical).vertical ? dPad.y : dPad.x;
}

Vector2f Control::getAxes(const InputState& state) const
{
	assert(isAxes());

	if (const auto* quad = std::get_if<ButtonQuad>(&m_physical))
		return {getPairAxis(quad->x, state), getPairAxis(quad->y, state)};
	if (const auto* stick = std::get_if<PadStick>(&m_physical))
		return state.getPad().getStick(stick->stick);
	return state.getPad().getDPad();
}

Vector2f Control::getMotion(const InputState& state) const
{
	assert(isMotion());

	if (is<MouseMotion>(m_physical))
		return state.mouseMotion();
	return state.getPad().getStick(std::get<PadStick>(m_physical).stick) * state.maxFrameStickTravel();
}

static bool isPressedBy(const Control::Digital& digital, const sf::Event& event, const InputState& state)
{
	if (const auto* key = std::get_if<Control::Key>(&digital))
	{
		const auto* const keyPressed = event.getIf<sf::Event::KeyPressed>();
		return keyPressed && keyPressed->code == key->code;
	}
	if (const auto* mouseButton = std::get_if<Control::MouseButton>(&digital))
	{
		const auto* const buttonPressed = event.getIf<sf::Event::MouseButtonPressed>();
		return buttonPressed && buttonPressed->button == mouseButton->button;
	}
	const auto* const padButtonPressed = event.getIf<sf::Event::JoystickButtonPressed>();
	const Gamepad& pad = state.getPad();
	return padButtonPressed && padButtonPressed->joystickId == pad.joystickId
	    && padButtonPressed->button == pad.layout->getButtonIndex(std::get<Control::PadButton>(digital).button);
}

float Control::getScroll(const sf::Event& event, const InputState& state) const
{
	assert(isScroll());

	if (is<MouseWheel>(m_physical))
	{
		const auto* const scrolled = event.getIf<sf::Event::MouseWheelScrolled>();
		return scrolled && scrolled->wheel == sf::Mouse::Wheel::Vertical ? scrolled->delta : 0;
	}
	const auto& pair = std::get<ButtonPair>(m_physical);
	return int(isPressedBy(pair.positive, event, state)) - int(isPressedBy(pair.negative, event, state));
}
