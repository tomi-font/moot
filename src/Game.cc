#include <Game.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CRigidbody.hh>

static void	populateWorld(World& world)
{
	Archetype*		arch = world.getArchetype(C(Component::Position) | C(Component::Render) | C(Component::Move) | C(Component::Player) | C(Component::CollisionBox) | C(Component::Rigidbody));
	sf::Vector2f	pos(200.f, 450.f);
	sf::Vector2f	size(100.f, 100.f);
	arch->get<CPosition>().emplace_back(pos);
	arch->get<CRender>().emplace_back(pos, size);
	arch->get<CMove>().emplace_back(1000);
	arch->get<CPlayer>().emplace_back(CPlayer::Controls({sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W}));
	arch->get<CCollisionBox>().emplace_back(sf::FloatRect(pos, size));
	arch->get<CRigidbody>().emplace_back();

	arch = world.getArchetype(C(Component::Position) | C(Component::Render) | C(Component::CollisionBox));
	pos.x = 100.f;
	pos.y = 800.f;
	size.x = 1600.f;
	arch->get<CPosition>().emplace_back(pos);
	arch->get<CRender>().emplace_back(pos, size);
	arch->get<CCollisionBox>().emplace_back(sf::FloatRect(pos, size));
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
