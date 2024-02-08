#include <World.hh>
#include <limits>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

constexpr float c_maxCoord = std::numeric_limits<float>::max() / 2;

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
	temp.add(CInput({ {sf::Keyboard::A, "left"}, {sf::Keyboard::D, "right"}, {sf::Keyboard::W, "jump"} },
		[](EntityHandle player, const std::string& key)
		{
			if (key == "jump")
			{
				CRigidbody*	rig = &player.get<CRigidbody>();

				// TODO: Decouple from CRigidbody's internal logic. Possibly by making an interface to it.
				if (rig->grounded)
				{
					rig->grounded = false;
					rig->velocity = -1000;
				}
			}
			else
			{
				const CInput& cInput = player.get<CInput>();
				player.get<CMove>().setMotion(cInput.isKeyPressed("right") - cInput.isKeyPressed("left"));
			}
		}));
	temp.add(CCollisionBox(sf::FloatRect(pos, size)));
	temp.add(CRigidbody());
	temp.add(CView(sf::FloatRect(-viewSize.x * .5f, -c_maxCoord, viewSize.x, c_maxCoord)));
	world.instantiate(temp);

	// Manual creation of a platform.
	size.x = std::numeric_limits<float>::max();
	size.y = std::numeric_limits<float>::min();
	pos = { -(size.x / 2), 0 };
	temp = {};
	temp.add(CPosition(pos));
	temp.add(CCollisionBox(sf::FloatRect(pos, size)));
	world.instantiate(temp);

	while (world.isRunning())
	{
		world.update();
	}
}
