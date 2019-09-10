#include <Game.hh>
#include <Component/CTransform.hh>
#include <Component/CRender.hh>

static void	populateWorld(World& world)
{
	Archetype&	arch = world.getArchetype(C(Component::Transform) | C(Component::Render));

	arch.get<CTransform>().emplace_back(sf::Vector2f(0,0));
	arch.get<CRender>().emplace_back();
}

static void	createWindow(sf::RenderWindow& window)
{
	sf::VideoMode	vm = sf::VideoMode::getDesktopMode();

	vm.width /= 2;
	vm.height /= 2;

	window.create(vm, "GAME");
	window.setPosition(sf::Vector2i(vm.width / 2, vm.height / 2));
}

Game::Game() : m_world(m_window)
{
	populateWorld(m_world);
	createWindow(m_window);
}
