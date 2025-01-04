#include <moot/parsing/GlobalFunctions.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Entity/PrototypeStore.hh>
#include <moot/Event/Event.hh>
#include <moot/Game.hh>
#include <moot/parsing/PrototypeAttributes.hh>
#include <moot/parsing/types.hh>
#include <moot/util/variant/indexToCompileTime.hh>
#include <moot/Window.hh>
#include <SFML/Window/Event.hpp>

static constexpr std::string_view PrototypeIdKey = "id";

static void parsePrototype(sol::table protoTable, Prototype* proto)
{
	for (const auto& attr : protoTable)
		PrototypeAttributes::parse(attr, proto);
}

static const Prototype* findOrMakePrototype(sol::table protoTable, PrototypeStore* prototypeStore, const sol::table& prototypeMetatable)
{
	auto idObj = protoTable[PrototypeIdKey];
	if (idObj.valid())
		return &prototypeStore->getPrototype(as<PrototypeStore::PrototypeId>(idObj));

	const auto [proto, id] = prototypeStore->newPrototype();

	parsePrototype(protoTable, proto);

	idObj = id;
	protoTable[sol::metatable_key] = prototypeMetatable;

	return proto;
}

void GlobalFunctions::registerAll(sol::state* lua, Game* game)
{
	PrototypeStore* const prototypeStore = game;
	EntityManager* const entityManager = game;
	Window* const window = game->window();

	lua->set_function("registerPrototype",
		[prototypeStore](std::string name, sol::table protoTable)
		{
			parsePrototype(protoTable, prototypeStore->newPrototype(std::move(name)));
		}
	);

	const sol::table prototypeMetatable = lua->create_table_with(sol::meta_method::garbage_collect,
		[prototypeStore](const sol::table& protoTable)
		{
			prototypeStore->deletePrototype(as<PrototypeStore::PrototypeId>(protoTable[PrototypeIdKey]));
		});
	const auto getPrototype =
		[=](sol::table protoTable)
		{
			return findOrMakePrototype(protoTable, prototypeStore, prototypeMetatable);
		};
	lua->set_function("spawn", sol::overload(
		[=](sol::table protoTable)
		{
			return entityManager->spawn(*getPrototype(protoTable));
		},
		[=](sol::table protoTable, const sol::object& posObj)
		{
			const auto pos = asVector2f(posObj);
			return entityManager->spawn(*getPrototype(protoTable), pos);
		},
		[=](const std::string& protoName)
		{
			return entityManager->spawn(prototypeStore->getPrototype(protoName));
		},
		[=](const std::string& protoName, const sol::object& posObj)
		{
			const auto pos = asVector2f(posObj);
			return entityManager->spawn(prototypeStore->getPrototype(protoName), pos);
		}
	));

	lua->set_function("remove", &Game::remove, game);

	lua->set_function("getEntity", &EntityManager::getEntity, entityManager);

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
			[game](sol::table, const std::string& name)
			{
				return game->properties()->get(name);
			}),
		sol::meta_method::new_index, sol::property(
			[game](sol::table, const std::string& name, const sol::object& value)
			{
				Properties* const properties = game->properties();
				variantIndexToCompileTime<Property::Value>(properties->get(name).index(),
					[&](auto I)
					{
						properties->set(name, asParsed<std::variant_alternative_t<I, Property::Value>>(value));
					}
				);
			})
	);

	lua->set_function("trigger",
		[game](Event::Id eventId, const EntityHandle& entity)
		{
			game->eventManager()->trigger({eventId, entity});
		});
}
