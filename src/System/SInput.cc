#include <System/SInput.hh>
#include <Entity/Entity.hh>
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
		for (Entity entity : m_groups[G::Input])
		{
			if (auto* callback = entity.get<CInput>().getCallback(event))
			{
				(*callback)(entity, event);
			}
		}
	}
}
