#include <Entity/Entity.hh>
#include <Game.hh>
#include <utility/Window.hh>

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
	world.instantiate(createPlatformBuilder());

	game.play();
}
