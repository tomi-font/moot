#include <Game.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>

static void	populateWorld(World& world)
{
	Archetype*	arch = world.getArchetype(C(Component::Position) | C(Component::Render) | C(Component::Move) | C(Component::Player));

	sf::Vector2f pos(100.f, 100.f);
	arch->get<CPosition>().emplace_back(pos);
	arch->get<CRender>().emplace_back(pos, pos);
	arch->get<CMove>().emplace_back(sf::Vector2f(1000.f, 0.f));
	arch->get<CPlayer>().emplace_back(CPlayer::Controls({sf::Keyboard::A, sf::Keyboard::D}));

	arch = world.getArchetype(C(Component::Position) | C(Component::Render));
	pos *= 4.f;
	arch->get<CPosition>().emplace_back(pos);
	arch->get<CRender>().emplace_back(pos, pos);
}

static void	createWindow(sf::RenderWindow& window)
{
	sf::VideoMode	vm = sf::VideoMode::getDesktopMode();
	vm.width /= 2;
	vm.height /= 2;

	window.create(vm, "GAME");
	window.setPosition(sf::Vector2i(vm.width / 2, vm.height / 2));
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
}

Game::Game() : m_world(m_window)
{
	populateWorld(m_world);
	createWindow(m_window);
}
