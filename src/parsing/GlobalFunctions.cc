#include <moot/parsing/GlobalFunctions.hh>
#include <moot/Entity/Entity.hh>
#include <moot/Entity/PrototypeStore.hh>
#include <moot/parsing/PrototypeAttributes.hh>
#include <moot/parsing/types.hh>
#include <moot/utility/variant/indexToCompileTime.hh>
#include <moot/Window.hh>

static constexpr std::string_view PrototypeUidKey = "uid";

static void parsePrototype(sol::table protoTable, Prototype* proto)
{
	for (const auto& attr : protoTable)
		PrototypeAttributes::parse(attr, proto);
}

static const Prototype& findOrMakePrototype(sol::table protoTable, PrototypeStore* prototypeStore, const sol::table& prototypeMetatable)
{
	auto uidObj = protoTable[PrototypeUidKey];
	if (uidObj.valid())
		return prototypeStore->getPrototype(as<PrototypeUid>(uidObj));

	const auto [proto, uid] = prototypeStore->newPrototype();

	parsePrototype(protoTable, proto);

	uidObj = uid;
	protoTable[sol::metatable_key] = prototypeMetatable;

	return *proto;
}

void GlobalFunctions::registerAll(sol::state* lua, World* world)
{
	PrototypeStore* const prototypeStore = world->prototypeStore();
	Window* const window = world->window();

	lua->set_function("register",
		[prototypeStore](std::string name, sol::table protoTable)
		{
			parsePrototype(protoTable, prototypeStore->newPrototype(std::move(name)));
		}
	);

	const sol::table prototypeMetatable = lua->create_table_with(sol::meta_method::garbage_collect,
		[prototypeStore](const sol::table& protoTable)
		{
			prototypeStore->deletePrototype(as<PrototypeUid>(protoTable[PrototypeUidKey]));
		});
	const auto getPrototype =
		[=](sol::table protoTable)
		{
			return findOrMakePrototype(protoTable, prototypeStore, prototypeMetatable);
		};
	lua->set_function("spawn", sol::overload(
		[=](sol::table protoTable)
		{
			world->spawn(getPrototype(protoTable));
		},
		[=](sol::table protoTable, const sol::object& pos)
		{
			world->spawn(getPrototype(protoTable), asVector2f(pos));
		},
		[world](const std::string& protoName)
		{
			world->spawn(protoName);
		},
		[world](const std::string& protoName, const sol::object& pos)
		{
			world->spawn(protoName, asVector2f(pos));
		}
	));
	lua->set_function("remove", &World::remove, world);

	lua->set_function("exitGame", &World::stopRunning, world);
	lua->set_function("findEntity", static_cast<std::optional<Entity> (World::*)(std::string_view) const>(&World::findEntity), world);
	lua->set_function("isKeyPressed",
		[](sf::Keyboard::Key key)
		{
			return static_cast<int>(sf::Keyboard::isKeyPressed(key));
		});
	lua->set_function("mapHudToWorld", &Window::mapHudToWorld<Vector2f>, window);
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
