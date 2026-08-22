#include <moot/Component/CInput.hh>
#include <cassert>

static bool operator==(const sf::Event& lhs, const sf::Event& rhs)
{
	if (lhs.is<sf::Event::Closed>())
		return rhs.is<sf::Event::Closed>();

	if (lhs.is<sf::Event::MouseMoved>())
		return rhs.is<sf::Event::MouseMoved>();

	if (lhs.is<sf::Event::KeyPressed>())
	{
		return rhs.is<sf::Event::KeyPressed>()
		    && lhs.getIf<sf::Event::KeyPressed>()->code
		    == rhs.getIf<sf::Event::KeyPressed>()->code;
	}
	if (lhs.is<sf::Event::KeyReleased>())
	{
		return rhs.is<sf::Event::KeyReleased>()
		    && lhs.getIf<sf::Event::KeyReleased>()->code
		    == rhs.getIf<sf::Event::KeyReleased>()->code;
	}

	if (lhs.is<sf::Event::MouseButtonPressed>())
	{
		return rhs.is<sf::Event::MouseButtonPressed>()
		    && lhs.getIf<sf::Event::MouseButtonPressed>()->button
		    == rhs.getIf<sf::Event::MouseButtonPressed>()->button;
	}
	if (lhs.is<sf::Event::MouseButtonReleased>())
	{
		return rhs.is<sf::Event::MouseButtonReleased>()
		    && lhs.getIf<sf::Event::MouseButtonReleased>()->button
		    == rhs.getIf<sf::Event::MouseButtonReleased>()->button;
	}

	if (lhs.is<sf::Event::MouseWheelScrolled>())
	{
		return rhs.is<sf::Event::MouseWheelScrolled>()
		    && lhs.getIf<sf::Event::MouseWheelScrolled>()->wheel
		    == rhs.getIf<sf::Event::MouseWheelScrolled>()->wheel;
	}

	assert(false);
}

const CInput::Watch::Callback* CInput::getCallback(const sf::Event& event) const
{
	const Watch::Callback* callback = nullptr;

	for (const Watch& watch : m_watches)
	{
		for (const sf::Event& watchEvent : watch.events)
		{
			if (watchEvent == event)
			{
				assert(!callback);
				callback = &watch.callback;
			}
		}
	}

	return callback;
}
