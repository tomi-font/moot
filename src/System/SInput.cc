#include <System/SInput.hh>
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

void SInput::update(float) const
{
	for (sf::Event event; m_window->pollEvent(event);)
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape)
			{
		case sf::Event::Closed:
				broadcast(Event::PlayerQuit);
				break;
			}
			[[fallthrough]];
		case sf::Event::KeyReleased:
			for (EntityHandle entity : m_groups[G::Input])
			{
				const CInput& cInput = entity.get<CInput>();

				if (cInput.controls.contains(event.key.code))
					cInput.callback(entity, cInput.controls.at(event.key.code));
			}
			break;
		}
	}
}
