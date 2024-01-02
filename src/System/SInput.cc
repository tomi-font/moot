#include <System/SInput.hh>
#include <Archetype.hh>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

// Indices for this system's groups.
enum G
{
	Player,
	COUNT
};

SInput::SInput()
{
	m_groups.resize(G::COUNT);
	m_groups[G::Player] = { CId<CPlayer> | CId<CMove> | CId<CRigidbody> };
}

static void	playerControls(sf::Keyboard::Key keyCode, Archetype* arch)
{
	const auto&	controls = arch->get<CPlayer>()[0].controls();

	if (keyCode == controls[CPlayer::Left] || keyCode == controls[CPlayer::Right])
	{
		CMove* cmov = &arch->get<CMove>()[0];
		const int direction = sf::Keyboard::isKeyPressed(controls[CPlayer::Right])
		                      - sf::Keyboard::isKeyPressed(controls[CPlayer::Left]);
		cmov->setMotion(direction);
	}
	else if (keyCode == controls[CPlayer::Jump])
	{
		CRigidbody*	rig = &arch->get<CRigidbody>()[0];

		// TODO: Decouple from CRigidbody's internal logic. Possibly by making an interface to it.
		if (rig->grounded)
		{
			rig->grounded = false;
			rig->velocity = -1000;
		}
	}
}

void SInput::update(float)
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
			playerControls(event.key.code, m_groups[G::Player].archs()[0]);
			break;
		}
	}
}
