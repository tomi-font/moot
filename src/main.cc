#include <World.hh>
#include <limits>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

static void addQuitControls(World& world)
{
	Template entity;

	entity.add(CInput({{
		{
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::Q},
			{.type = sf::Event::Closed},
		},
		[&world](EntityHandle, const sf::Event&)
		{
			world.stopRunning();
		}
	}}));

	world.instantiate(entity);
}

static CInput::Watch moveInputWatch()
{
	return {
		{
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::A},
		  	{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::D},
			{.type = sf::Event::KeyReleased, .key.code = sf::Keyboard::A},
		  	{.type = sf::Event::KeyReleased, .key.code = sf::Keyboard::D},
		},
		[](EntityHandle player, const sf::Event&)
		{
			player.get<CMove>().setMotion(
				sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A));
		}
	};
}

static CInput::Watch jumpInputWatch()
{
	return {
		{ {.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::W} },
		[](EntityHandle player, const sf::Event&)
		{
			// TODO: Decouple from CRigidbody's internal logic. Possibly by making an interface to it.
			CRigidbody* rig = &player.get<CRigidbody>();

			rig->grounded = false;
			rig->velocity = -1000;
		}
	};
}

int	main()
{
	// Manual creation of the window.
	auto vm = sf::VideoMode::getDesktopMode();
	vm.width /= 2;
	vm.height /= 2;
	sf::RenderWindow window(vm, "demo");
	window.setPosition(sf::Vector2i(vm.width / 2, vm.height / 2));
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);

	World world(&window);

	// Manual creation of a player.
	const sf::Vector2f viewSize(vm.width, vm.height);
	sf::Vector2f pos(0, -viewSize.y);
	sf::Vector2f size(100.f, 100.f);
	Template temp;
	temp.add(CPosition(pos));
	temp.add(CRender(pos, size, sf::Color::White));
	temp.add(CMove(1000));
	temp.add(CInput({moveInputWatch(), jumpInputWatch()}));
	temp.add(CCollisionBox(sf::FloatRect(pos, size)));
	temp.add(CRigidbody());
	temp.add(CView(sf::FloatRect(-viewSize.x * .5f, -viewSize.y, viewSize.x, viewSize.y)));
	world.instantiate(temp);

	// Manual creation of a platform.
	size.x = std::numeric_limits<float>::max();
	size.y = std::numeric_limits<float>::min();
	pos = { -(size.x / 2), 0 };
	temp = {};
	temp.add(CPosition(pos));
	temp.add(CCollisionBox(sf::FloatRect(pos, size)));
	world.instantiate(temp);

	addQuitControls(world);

	while (world.isRunning())
	{
		world.update();
	}
}
