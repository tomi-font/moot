#pragma once

#include <moot/Input/Control.hh>
#include <moot/struct/Vector2.hh>
#include <functional>
#include <variant>
#include <vector>

struct EntityHandle;

// A logical input: the value of a type, gathered from any number of controls and handed to a callback.
class Binding
{
public:

	enum class Type
	{
		Button,  // Whether any control is pressed.
		Axis,    // From -1 to 1.
		Axes,    // A vector of length 1 at most, up being +y.
		Pointer, // Where the pointer is, in pixels.
		Motion,  // The motion of the frame in pixels, up being +y.
		Scroll   // The notches an event scrolls by.
	};

	using Value = std::variant<bool, float, Vector2f, Vector2i>;
	using Callback = std::function<void (EntityHandle&, const Value&)>;

	Binding(Type, std::vector<Control>&&);

	void setCallback(Callback&& callback) { m_callback = std::move(callback); }

	bool hasKey(sf::Keyboard::Key) const;

	static bool deliversOn(const sf::Event&);

	// A press, a release or a scroll, delivered as it happens so that none is lost within a frame.
	void deliverEventUpdate(const sf::Event&, const InputState&, EntityHandle&);
	// Whatever changed over the frame.
	void deliverFrameUpdate(const InputState&, EntityHandle&);

private:

	bool isAnyPressed(const InputState&) const;

	void deliver(const Value&, EntityHandle&);
	void deliverIfChanged(const Value&, EntityHandle&);

	Type m_type;
	std::vector<Control> m_controls;
	Callback m_callback;
	Value m_lastValue;
};
