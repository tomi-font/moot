#include <Game.hh>

Game::Game() : m_world(&m_window)
{
	sf::VideoMode halfScreen = sf::VideoMode::getDesktopMode();
	halfScreen.width /= 2;
	halfScreen.height /= 2;
	
	m_window.create(halfScreen, "game");
	m_window.setPosition(sf::Vector2i(halfScreen.width / 2, halfScreen.height / 2));
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(false);
}

void Game::create()
{
	m_factory.populateWorld(&m_world);
}

void Game::play()
{
	// Restart the clock to not count the setup time.
	m_world.restartClock();

	while (m_world.isRunning())
		m_world.update();
}
