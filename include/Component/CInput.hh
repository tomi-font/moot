#pragma once

#include <functional>
#include <SFML/Window/Event.hpp>

class EntityHandle;

class CInput
{
public:

	struct Watch
	{
		// The events of interest (for each, the type and specific data that must match).
		std::vector<sf::Event> events;

		using Callback = std::function<void (EntityHandle, const sf::Event&)>;
		Callback callback;
	};

	CInput(std::vector<Watch>&& watches) : m_watches(std::move(watches)) {}

	const Watch::Callback* getCallback(const sf::Event&) const;

private:

	const std::vector<Watch> m_watches;
};
