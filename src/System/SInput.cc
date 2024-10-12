#include <moot/System/SInput.hh>
#include <moot/Entity/Entity.hh>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

// Indices for this system's queries.
enum Q
{
	Input,
	COUNT
};

SInput::SInput() :
	System(Q::COUNT)
{
	m_queries.resize(Q::COUNT);
	m_queries[Q::Input] = { CId<CInput> };
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

		for (Entity entity : m_queries[Q::Input])
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
