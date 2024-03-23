#include <Game.hh>

Game::Game() : m_world(&m_window)
{
	sf::VideoMode halfScreen = sf::VideoMode::getDesktopMode();
	halfScreen.width /= 2;
	halfScreen.height /= 2;
	
	m_window.create(halfScreen, "");
	m_window.setPosition(sf::Vector2i(halfScreen.width / 2, halfScreen.height / 2));
	m_window.setVerticalSyncEnabled(true);
	m_window.setKeyRepeatEnabled(false);
}
