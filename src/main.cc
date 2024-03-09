#include <Entity/Entity.hh>
#include <World.hh>
#include <utility/Window.hh>
#include <limits>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

static Template createQuitControls()
{
	return Template().add(CInput({{
		{
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::Q},
			{.type = sf::Event::Closed},
		},
		[](const Entity& entity, const sf::Event&)
		{
			entity.world()->stopRunning();
		}
	}}));
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
			player.get<CRigidbody*>()->applyForce(750 * (event.key.code == sf::Keyboard::W ? 1 : -1));
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
	sf::Vector2f pos(0, viewSize.y);
	sf::Vector2f size(100.f, 100.f);
	Template temp;
	temp.add<CPosition>(pos);
	temp.add<CRender>(pos, size, sf::Color::White);
	temp.add<CMove>(1000);
	temp.add(CInput({moveInputWatch(), jumpInputWatch()}));
	temp.add<CCollisionBox>(pos, size);
	temp.add<CRigidbody>();
	temp.add<CView>(viewSize, sf::FloatRect(-viewSize.x * .5f, viewSize.y * 2, viewSize.x, viewSize.y * 2));
	return temp;
}

static Template createGround()
{
	const sf::Vector2f size = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
	const sf::Vector2f pos = { -(size.x / 2), 0 };

	return Template()
		.add<CPosition>(pos)
		.add<CCollisionBox>(pos, size);
}

static Template createPlatformBuilder()
{
	return Template().add(CInput({
		{
			{ {.type = sf::Event::MouseButtonPressed, .mouseButton.button = sf::Mouse::Button::Left} },
			[](const Entity& entity, const sf::Event& event)
			{
				if (entity.world()->findEntity("platformInConstruction"))
					return;

				Template platform;

				platform.add<CHudRender>(Window::mapPixelToHud(event.mouseButton), sf::Color::Black);
				platform.add<CName>("platformInConstruction");

				entity.world()->instantiate(std::move(platform));
			}
		},
		{
			{ {.type = sf::Event::MouseMoved} },
			[](const Entity& entity, const sf::Event& event)
			{
				Entity platform = entity.world()->findEntity("platformInConstruction");
				if (!platform)
					return;
				CHudRender* cHudRender = platform.get<CHudRender*>();

				cHudRender->resize(Window::mapPixelToHud(event.mouseMove) - cHudRender->position());
			}
		},
		{
			{ {.type = sf::Event::MouseButtonReleased, .mouseButton.button = sf::Mouse::Button::Left} },
			[](const Entity& entity, const sf::Event& event)
			{
				Entity platform = entity.world()->findEntity("platformInConstruction");
				assert(platform);
				const sf::Vector2f pos = Window::mapHudToWorld(platform.get<CHudRender>().position());
				const sf::Vector2f size = Window::mapPixelToWorld(event.mouseButton) - pos;

				platform.remove<CName>();
				platform.remove<CHudRender>();
				platform.add<CPosition>(pos);
				platform.add<CCollisionBox>(pos, size);
				platform.add<CRender>(pos, size, sf::Color::Black);
			}
		},
	}));
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
