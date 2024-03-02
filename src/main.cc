#include <Entity/Entity.hh>
#include <World.hh>
#include <limits>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

static Template createQuitControls()
{
	Template temp;

	temp.add(CInput({{
		{
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::Q},
			{.type = sf::Event::Closed},
		},
		[](const Entity& entity, const sf::Event&)
		{
			entity.world()->stopRunning();
		}
	}}));

	return temp;
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
		[](Entity& player, const sf::Event&)
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
		[](Entity& player, const sf::Event& event)
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
	temp.add(CCollisionBox({pos, size}));
	temp.add(CRigidbody());
	temp.add(CView(viewSize, sf::FloatRect(-viewSize.x * .5f, -viewSize.y * 2, viewSize.x, viewSize.y * 2)));
	return temp;
}

static Template createGround()
{
	Template temp;
	sf::Vector2f size = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	CPosition cPos = { -(size.x / 2), 0 };

	temp.add(cPos);
	temp.add(CCollisionBox({cPos, size}));
	return temp;	
}

static Template createPlatformBuilder()
{
	Template temp;

	temp.add(CInput({
		{
			{ {.type = sf::Event::MouseButtonPressed, .mouseButton.button = sf::Mouse::Button::Left} },
			[](const Entity& entity, const sf::Event& event)
			{
				if (entity.world()->findEntity("platformInConstruction"))
					return;
				
				World* world = entity.world();
				const CPosition cPos = world->window().mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
				Template platform;
				
				platform.add(cPos);
				platform.add(CRender(cPos, {}, sf::Color::Black));
				platform.add(CName("platformInConstruction"));

				world->instantiate(std::move(platform));
			}
		},
		{
			{ {.type = sf::Event::MouseMoved} },
			[](const Entity& entity, const sf::Event& event)
			{
				World* world = entity.world();
				Entity platform = world->findEntity("platformInConstruction");
				if (!platform)
					return;

				const sf::Vector2f mousePos = world->window().mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
				sf::Vector2f newSize = mousePos - platform.get<CPosition>();
				newSize.x = std::max(1.f, newSize.x);
				newSize.y = std::max(1.f, newSize.y);

				platform.resize(newSize);
			}
		},
		{
			{ {.type = sf::Event::MouseButtonReleased, .mouseButton.button = sf::Mouse::Button::Left} },
			[](const Entity& entity, const sf::Event&)
			{
				World* world = entity.world();
				Entity platform = world->findEntity("platformInConstruction");
				assert(platform);

				platform.remove<CName>();
				platform.add(CCollisionBox(platform.get<CPosition>(), platform.get<CRender>().getSize()));
			}
		},
	}));
	return temp;
}

int	main()
{
	sf::RenderWindow window;
	configureWindow(window);

	World world(&window);

	world.instantiate(createPlayer(window.getView().getSize()));
	world.instantiate(createGround());
	world.instantiate(createPlatformBuilder());
	world.instantiate(createQuitControls());

	while (world.isRunning())
	{
		world.update();
	}
}
