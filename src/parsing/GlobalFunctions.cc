#include <moot/parsing/GlobalFunctions.hh>
#include <moot/Entity/Entity.hh>
#include <moot/Entity/PrototypeStore.hh>
#include <moot/parsing/PrototypeAttributes.hh>
#include <moot/parsing/types.hh>
#include <moot/utility/variant/indexToCompileTime.hh>

static constexpr std::string_view PrototypeUidKey = "uid";

static const Prototype& findOrMakePrototype(sol::table componentTable, PrototypeStore* prototypeStore, const sol::table& prototypeMetatable)
{
	auto uidObj = componentTable[PrototypeUidKey];
	if (uidObj.valid())
		return prototypeStore->getPrototype(as<PrototypeUid>(uidObj));

	const auto [proto, uid] = prototypeStore->newPrototype();

	for (const auto& component : componentTable)
		PrototypeAttributes::parse(component, proto);

	uidObj = uid;
	componentTable[sol::metatable_key] = prototypeMetatable;

	return *proto;
}

void GlobalFunctions::registerAll(sol::state* lua, World* world, PrototypeStore* prototypeStore)
{
	Window* const window = world->window();

	lua->set_function("exitGame", &World::stopRunning, world);

	const sol::table prototypeMetatable = lua->create_table_with(sol::meta_method::garbage_collect,
		[prototypeStore](const sol::table& proto)
		{
			prototypeStore->deletePrototype(as<PrototypeUid>(proto[PrototypeUidKey]));
		});
	const auto getPrototype =
		[=](sol::table componentTable)
		{
			return findOrMakePrototype(componentTable, prototypeStore, prototypeMetatable);
		};
	lua->set_function("spawn", sol::overload(
		[=](sol::table entity)
		{
			world->instantiate(getPrototype(entity));
		},
		[=](sol::table entity, sol::table pos)
		{
			world->instantiate(getPrototype(entity), asVector2f(pos));
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
			[world](sol::table, const std::string& name)
			{
				return world->properties()->get(name);
			}),
		sol::meta_method::new_index, sol::property(
			[world](sol::table, const std::string& name, const sol::object& value)
			{
				Properties* const properties = world->properties();
				variantIndexToCompileTime<Property::Value>(properties->get(name).index(),
					[&](auto I)
					{
						properties->set(name, asParsed<std::variant_alternative_t<I, Property::Value>>(value));
					}
				);
			})
	);
}
