#include <moot/parsing/GlobalFunctions.hh>
#include <moot/Entity/Handle.hh>
#include <moot/Entity/Prototype.hh>
#include <moot/Event/Event.hh>
#include <moot/Game.hh>
#include <moot/parsing/PrototypeAttributes.hh>
#include <moot/parsing/types.hh>
#include <moot/Resource/Store.hh>
#include <moot/util/variant/indexToCompileTime.hh>
#include <moot/Window.hh>

static constexpr std::string_view PrototypeIdKey = "id";

static void parsePrototype(const sol::table& protoTable, EntityPrototype* proto)
{
	for (const auto& attr : protoTable)
		PrototypeAttributes::parse(attr, proto);
}

static const EntityPrototype& findOrMakePrototype(sol::table& protoTable, ResourceStore* resources, const sol::table& prototypeMetatable)
{
	auto idObj = protoTable[PrototypeIdKey];
	if (idObj.valid())
		return resources->get<EntityPrototype>(as<ResourceStore::Id>(idObj));

	const auto [id, proto] = resources->add<EntityPrototype>();

	parsePrototype(protoTable, proto);

	idObj = id;
	protoTable[sol::metatable_key] = prototypeMetatable;

	return *proto;
}

void GlobalFunctions::registerAll(sol::state* lua, Game* game)
{
	ResourceStore* const resources = game;
	EntityManager* const entityManager = game;
	Window* const window = game->window();

	const sol::table prototypeMetatable = lua->create_table_with(sol::meta_method::garbage_collect,
		[resources](const sol::table& protoTable)
		{
			resources->remove<EntityPrototype>(as<ResourceStore::Id>(protoTable[PrototypeIdKey]));
		});
	const auto getPrototype =
		[=](sol::table& protoTable)
		{
			return findOrMakePrototype(protoTable, resources, prototypeMetatable);
		};
	lua->set_function("spawn", sol::overload(
		[=](sol::table protoTable)
		{
			return entityManager->spawn(getPrototype(protoTable));
		},
		[=](sol::table protoTable, const sol::object& posObj)
		{
			const auto pos = asVector2f(posObj);
			return entityManager->spawn(getPrototype(protoTable), pos);
		},
		[=](const std::string& protoName)
		{
			return entityManager->spawn(resources->get<EntityPrototype>(protoName));
		},
		[=](const std::string& protoName, const sol::object& posObj)
		{
			const auto pos = asVector2f(posObj);
			return entityManager->spawn(resources->get<EntityPrototype>(protoName), pos);
		}
	));

	lua->set_function("remove", &EntityManager::remove, entityManager);

	lua->set_function("getEntity", &EntityManager::getEntity, entityManager);

	lua->set_function("mapHudToWorld", &Window::mapHudToWorld, window);
	lua->set_function("mapPixelToHud", &Window::mapPixelToHud, window);
	lua->set_function("mapPixelToWorld", &Window::mapPixelToWorld, window);

	lua->create_named_table("properties")[sol::metatable_key] = lua->create_table_with(
		sol::meta_method::index, sol::property(
			[game](const sol::table&, const std::string& name, sol::this_state solState) -> sol::object
			{
				if (const auto value = game->properties()->find(name))
					return sol::make_object(solState.lua_state(), *value);
				else
					return sol::make_object(solState.lua_state(), sol::lua_nil);
			}),
		sol::meta_method::new_index, sol::property(
			[game](const sol::table&, const std::string& name, const sol::object& valueObj)
			{
				Properties* const properties = game->properties();
				if (const auto current = properties->find(name))
				{
					variantIndexToCompileTime<Property::Value>(current->index(),
						[&](auto I)
						{
							properties->set(name, asParsed<std::variant_alternative_t<I, Property::Value>>(valueObj));
						}
					);
				}
				else
					properties->set(name, valueObj.as<Property::Value>());
			})
	);

	lua->set_function("trigger",
		[game](Event::Id eventId, const EntityHandle& entity)
		{
			game->eventManager()->trigger({eventId, entity});
		});
}
