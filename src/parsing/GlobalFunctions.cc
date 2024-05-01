#include <parsing/GlobalFunctions.hh>
#include <Entity/Entity.hh>
#include <Entity/TemplateStore.hh>
#include <parsing/ComponentAttributes.hh>
#include <parsing/types.hh>
#include <utility/Window.hh>

static const Template& getTemplate(sol::table componentTable, TemplateStore* templateStore)
{
	auto uidObj = componentTable["uid"];
	if (uidObj.valid())
		return templateStore->getTemplate(uidObj.get<unsigned>());

	const auto [temp, uid] = templateStore->newTemplate();

	for (const auto& component : componentTable)
		ComponentAttributes::parse(component, temp);

	uidObj = uid;
	return *temp;
}

void GlobalFunctions::registerPrePopulating(sol::state* lua, World* world, TemplateStore* templateStore)
{
	lua->set_function("spawn", sol::overload(
		[=](sol::table entity)
		{
			world->instantiate(getTemplate(entity, templateStore));
		},
		[=](sol::table entity, sol::table pos)
		{
			world->instantiate(getTemplate(entity, templateStore), asVector2f(pos));
		}
	));
	lua->set_function("exitGame", &World::stopRunning, world);
}

void GlobalFunctions::registerPostPopulating(sol::state* lua, World* world)
{
	lua->set_function("findEntity", &World::findEntity, world);
	lua->set_function("isKeyPressed",
		[](sf::Keyboard::Key key)
		{
			return static_cast<int>(sf::Keyboard::isKeyPressed(key));
		});
	lua->set_function("mapHudToWorld", &Window::mapHudToWorld<sf::Vector2f>);
	lua->set_function("mapPixelToHud", sol::overload(
		&Window::mapPixelToHud<sf::Event::MouseButtonEvent>,
		&Window::mapPixelToHud<sf::Event::MouseMoveEvent>
	));
	lua->set_function("mapPixelToWorld", &Window::mapPixelToWorld<sf::Event::MouseButtonEvent>);
}
