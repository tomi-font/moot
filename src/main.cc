#include <World.hh>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

int	main()
{
	World world;

	// Manual creation of a player.
	sf::Vector2f pos(200.f, 650.f);
	sf::Vector2f size(100.f, 100.f);
	Template temp;
	temp.add(CPosition(pos));
	temp.add(CRender(pos, size, sf::FloatRect(0, 0, 2, 2)));
	temp.add(CMove(1000));
	temp.add(CPlayer(CPlayer::Controls({sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W})));
	temp.add(CCollisionBox(sf::FloatRect(pos, size)));
	temp.add(CRigidbody());
	world.instantiate(temp);

	// Manual creation of a platform.
	pos = {100, 800};
	size.x = 1600.f;
	temp = {};
	temp.add(CPosition(pos));
	temp.add(CRender(pos, size, sf::FloatRect(0, 2, 32, 2)));
	temp.add(CCollisionBox(sf::FloatRect(pos, size)));
	world.instantiate(temp);

	// Manual creation of the window.
	auto vm = sf::VideoMode::getDesktopMode();
	vm.width /= 2;
	vm.height /= 2;
	sf::RenderWindow window(vm, "GAME");
	window.setPosition(sf::Vector2i(vm.width / 2, vm.height / 2));
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	while (world.isRunning())
	{
		world.update(window);
	}
}
