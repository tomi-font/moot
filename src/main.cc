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
			player.get<CMove*>()->setMotion(
				sf::Keyboard::isKeyPressed(sf::Keyboard::D) - sf::Keyboard::isKeyPressed(sf::Keyboard::A));
		}
	};
}

static CInput::Watch jumpInputWatch()
{
	return {
		{
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::W},
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::S},
		},
		[](EntityHandle player, const sf::Event& event)
		{
			player.get<CRigidbody*>()->applyForce((event.key.code == sf::Keyboard::W) ? -500 : 1000);
		}
	};
}

static void configureWindow(sf::RenderWindow& window)
{
	sf::VideoMode halfScreen = sf::VideoMode::getDesktopMode();
	halfScreen.width /= 2;
	halfScreen.height /= 2;
	
	window.create(halfScreen, "demo");
	window.setPosition(sf::Vector2i(halfScreen.width / 2, halfScreen.height / 2));
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
}

static Template createPlayer(const sf::Vector2f& viewSize)
{
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
	return temp;
}

static Template createGround()
{
	Template temp;
	sf::Vector2f size = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	CPosition cPos = {{ -(size.x / 2), 0 }};

	temp.add(cPos);
	temp.add(CCollisionBox(sf::FloatRect(cPos, size)));
	return temp;	
}

int	main()
{
	sf::RenderWindow window;
	configureWindow(window);

	World world(&window);

	world.instantiate(createPlayer(window.getView().getSize()));
	world.instantiate(createGround());

	addQuitControls(world);

	while (world.isRunning())
	{
		world.update();
	}
}
