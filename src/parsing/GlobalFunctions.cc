#include <moot/parsing/GlobalFunctions.hh>
#include <moot/Entity/Entity.hh>
#include <moot/Entity/TemplateStore.hh>
#include <moot/parsing/TemplateAttributes.hh>
#include <moot/parsing/types.hh>

static constexpr std::string_view c_templateUidKey = "uid";

static const Template& findOrMakeTemplate(sol::table componentTable, TemplateStore* templateStore, const sol::table& templateMetatable)
{
	auto uidObj = componentTable[c_templateUidKey];
	if (uidObj.valid())
		return templateStore->getTemplate(as<TemplateUid>(uidObj));

	const auto [temp, uid] = templateStore->newTemplate();

	for (const auto& component : componentTable)
		TemplateAttributes::parse(component, temp);

	uidObj = uid;
	componentTable[sol::metatable_key] = templateMetatable;

	return *temp;
}

void GlobalFunctions::registerAll(sol::state* lua, World* world, TemplateStore* templateStore)
{
	Window* const window = world->window();

	lua->set_function("exitGame", &World::stopRunning, world);

	const sol::table templateMetatable = lua->create_table_with(sol::meta_method::garbage_collect,
		[templateStore](const sol::table& temp)
		{
			templateStore->deleteTemplate(as<TemplateUid>(temp[c_templateUidKey]));
		});
	const auto getTemplate =
		[=](sol::table componentTable)
		{
			return findOrMakeTemplate(componentTable, templateStore, templateMetatable);
		};
	lua->set_function("spawn", sol::overload(
		[=](sol::table entity)
		{
			world->instantiate(getTemplate(entity));
		},
		[=](sol::table entity, sol::table pos)
		{
			world->instantiate(getTemplate(entity), asVector2f(pos));
		}
	));

	lua->set_function("findEntity", &World::findEntity, world);
	lua->set_function("isKeyPressed",
		[](sf::Keyboard::Key key)
		{
			return static_cast<int>(sf::Keyboard::isKeyPressed(key));
		});
	lua->set_function("mapHudToWorld", &Window::mapHudToWorld<sf::Vector2f>, window);
	lua->set_function("mapPixelToHud", sol::overload(
		[window](const sf::Event::MouseButtonEvent& mouseButton)
		{
			return window->mapPixelToHud(mouseButton);
		},
		[window](const sf::Event::MouseMoveEvent& mouseMove)
		{
			return window->mapPixelToHud(mouseMove);
		}
	));
	lua->set_function("mapPixelToWorld", &Window::mapPixelToWorld<sf::Event::MouseButtonEvent>, window);

	lua->create_named_table("properties")[sol::metatable_key] = lua->create_table_with(
		sol::meta_method::index, sol::property(
			[world](sol::table, std::string_view name)
			{
				return world->propertyManager()->get(name);
			}),
		sol::meta_method::new_index, sol::property(
			[world](sol::table, std::string_view name, const Property::Value& value)
			{
				world->propertyManager()->set(name, value);
			})
	);
}
