#include <Entity/Entity.hh>
#include <Game.hh>
#include <utility/Window.hh>

static CInput::Watch moveInputWatch()
{
	return {
		{
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::A},
			{.type = sf::Event::KeyPressed, .key.code = sf::Keyboard::D},
			{.type = sf::Event::KeyReleased, .key.code = sf::Keyboard::A},
			{.type = sf::Event::KeyReleased, .key.code = sf::Keyboard::D},
		},
		[](const Entity& player, const sf::Event&)
		{
			player.get<CMove*>()->setMotionX(
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
		[](const Entity& player, const sf::Event& event)
		{
			player.get<CRigidbody*>()->applyYForce(750 * (event.key.code == sf::Keyboard::W ? 1 : -1));
		}
	};
}

static CInput::Watch zoomInputWatch()
{
	return {
		{ {.type = sf::Event::MouseWheelScrolled, .mouseWheelScroll.wheel = sf::Mouse::Wheel::VerticalWheel } },
		[](const Entity& entity, const sf::Event& event)
		{
			entity.get<CView*>()->zoom(1 - event.mouseWheelScroll.delta / 4);
		}
	};
}

static Template createPlayer(const sf::Vector2f& viewSize)
{
	sf::Vector2f size(100.f, 100.f);
	Template temp;
	temp.add<CRender>(size, sf::Color::White);
	temp.add<CMove>(1000);
	temp.add(CInput({moveInputWatch(), jumpInputWatch(), zoomInputWatch()}));
	temp.add<CCollisionBox>(size);
	temp.add<CRigidbody>();
	temp.add<CView>(viewSize, FloatRect({-viewSize.x, 0}, viewSize * 2.f));
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

				Template platform;

				platform.add(CHudRender(Window::mapPixelToHud(event.mouseButton), {}, sf::Color::Black));
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
				if (!platform)
					return;
				const sf::Vector2f pos = Window::mapHudToWorld(platform.get<CHudRender>().position());
				const sf::Vector2f size = Window::mapPixelToWorld(event.mouseButton) - pos;

				platform.remove<CName>();
				platform.remove<CHudRender>();
				platform.add<CPosition>(pos);
				platform.add<CCollisionBox>(size);
				platform.add<CRender>(size, sf::Color::Black);
			}
		},
	}));
	return temp;
}

int	main()
{
	Game game;
	game.create();

	World& world = *game.world();
	const sf::RenderWindow& window = *game.window();
	const auto& viewSize = window.getView().getSize();
	
	world.instantiate(createPlayer(viewSize), {0, viewSize.y});
	world.instantiate(createPlatformBuilder());

	game.play();
}
