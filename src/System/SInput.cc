#include <System/SInput.hh>
#include <Component/CPlayer.hh>
#include <Component/CMove.hh>
#include <Component/CRigidbody.hh>
#include <SFML/Window/Event.hpp>

// indices for m_groups
enum G
{
	Player,
	COUNT
};

SInput::SInput(EventManager& em) : EventListener(em)
{
	m_groups.reserve(G::COUNT);
	m_groups.emplace_back(CId<CPlayer> | CId<CMove>);
}

static void	playerControls(sf::Keyboard::Key keyCode, Archetype* arch)
{
	const auto&	controls = arch->get<CPlayer>()[0].getControls();
	
	if (keyCode == controls[CPlayer::Left] || keyCode == controls[CPlayer::Right])
	{
		CMove*	move = &arch->get<CMove>()[0];
		int		direction = sf::Keyboard::isKeyPressed(controls[CPlayer::Right]) - sf::Keyboard::isKeyPressed(controls[CPlayer::Left]);

		if (direction)
			move->setVelocity(sf::Vector2f(static_cast<float>(direction * move->getSpeed()), 0.f));
		move->setMoving(direction);
	}
	else if (keyCode == controls[CPlayer::Jump])
	{
		CRigidbody*	rig = &arch->get<CRigidbody>()[0];

		if (rig->isGrounded())
		{
			rig->setGrounded(false);
			rig->setVelocity(-1000);
		}
	}
}

void	SInput::update(sf::RenderWindow& window, float)
{
	for (sf::Event event; window.pollEvent(event);)
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Escape)
			{
		case sf::Event::Closed:
				trigger(Event::PlayerQuit);
				break;
			}
			[[fallthrough]];
		case sf::Event::KeyReleased:
			playerControls(event.key.code, m_groups[G::Player].archs[0]);
			break;

		}
	}
}
