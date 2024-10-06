#include <moot/System/SInput.hh>
#include <moot/Entity/Entity.hh>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

// Indices for this system's groups.
enum G
{
	Input,
	COUNT
};

SInput::SInput()
{
	m_groups.resize(G::COUNT);
	m_groups[G::Input] = { CId<CInput> };
}

void SInput::initializeProperties()
{
	registerPropertyGetter("windowSize", [this](){ return sf::Vector2f(m_window->getSize()); });
}

void SInput::update(float) const
{
	for (sf::Event event; m_window->pollEvent(event);)
	{
		bool eventHasCallback = false;

		for (Entity entity : m_groups[G::Input])
		{
			if (auto* callback = entity.get<CInput>().getCallback(event))
			{
				(*callback)(entity, event);
				eventHasCallback = true;
			}
		}

		if (!eventHasCallback
		    && (event.type == sf::Event::Closed
		        || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)))
		{
			broadcast({.type = Event::GameClose});
		}
	}
}
