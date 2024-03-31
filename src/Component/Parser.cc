#include <Component/Parser.hh>
#include <Entity/Entity.hh>

static void registerCInput(sol::state* lua)
{
	sol::table event = lua->create_table("Event");

	event["WindowClosed"] = sf::Event(sf::Event::Closed);
	
	event["KeyPressed"] = [](sf::Keyboard::Key keyCode)
	{
		return sf::Event{sf::Event::KeyPressed, .key.code = keyCode};
	};
	lua->new_enum("Key", "Q", sf::Keyboard::Key::Q);
}

void ComponentParser::registerAll(sol::state* lua)
{
	registerCInput(lua);
}

using ComponentData = ComponentParser::Data;

template<typename T> static T as(const ComponentData& var)
{
	assert(var.valid());
	assert(var.get_type() == sol::lua_type_of_v<T>);
	return var.as<T>();
}

static sol::table asLuaMap(const ComponentData& var)
{
	return as<sol::table>(var);
}

static sol::table asLuaArray(const ComponentData& var)
{
	const auto& luaArray = asLuaMap(var);
	for (const auto& [luaIndex, value] : luaArray)
	{
		const auto index = as<unsigned>(luaIndex);
		assert(index > 0 && index <= luaArray.size());
		assert(value.valid());
	}
	return luaArray;
}
template<unsigned N> static sol::table asLuaArray(const ComponentData& var)
{
	const auto& luaArray = asLuaArray(var);
	const auto size = luaArray.size();
	assert(size == N);
	return luaArray;
}
template<typename T, unsigned N> static std::array<T, N> asArray(const ComponentData& var)
{
	std::array<T, N> array;

	for (const auto& [luaIndex, value] : asLuaArray<N>(var))
		array[as<unsigned>(luaIndex) - 1] = as<T>(value);

	return array;
}
template<typename T> static std::vector<T> asArray(const ComponentData& var)
{
	const auto& luaArray = asLuaArray(var);
	std::vector<T> vector(luaArray.size());

	for (const auto& [luaIndex, value] : luaArray)
		vector[as<unsigned>(luaIndex) - 1] = as<T>(value);

	return vector;
}

template<typename T> static sf::Vector2<T> asVector2(const ComponentData& var)
{
	const auto& array = asArray<T, 2>(var);
	return {array[0], array[1]};
}
static auto asVector2f(const ComponentData& var)
{
	return asVector2<float>(var);
}

template<typename T> static Rect<T> asRect(const ComponentData& var)
{
	const auto& array = asArray<T, 4>(var);
	return {array[0], array[1], array[2], array[3]};
}
static auto asFloatRect(const ComponentData& var)
{
	return asRect<float>(var); 
}

template<typename T> static void parser(Template*, const ComponentData&);

template<> void parser<CPosition>(Template* entity, const ComponentData& data)
{
	entity->add<CPosition>(asVector2f(data));
}

template<> void parser<CRender>(Template* entity, const ComponentData& data)
{
	const auto& map = asLuaMap(data);
	entity->add<CRender>(asVector2f(map["pos"]), asVector2f(map["size"]), sf::Color::Black);
}

template<> void parser<CMove>(Template* entity, const ComponentData& data)
{
	const auto& map = asLuaMap(data);
	entity->add<CMove>(as<unsigned>(map["speed"]));
}

template<> void parser<CInput>(Template* entity, const ComponentData& data)
{
	std::vector<CInput::Watch> watches;

	for (const auto& [_, value] : asLuaArray(data))
	{
		const auto& watch = asLuaArray<2>(value);

		watches.emplace_back(asArray<sf::Event>(watch[1]), as<sol::function>(watch[2]));
	}
	entity->add<CInput>(std::move(watches));
}

template<> void parser<CCollisionBox>(Template* entity, const ComponentData& data)
{
	const auto& map = asLuaMap(data);
	entity->add<CCollisionBox>(asVector2f(map["pos"]), asVector2f(map["size"]));
}

template<> void parser<CRigidbody>(Template* entity, const ComponentData& data)
{
	assert(asLuaMap(data).empty());
	entity->add<CRigidbody>();
}

template<> void parser<CView>(Template* entity, const ComponentData& data)
{
	const auto& map = asLuaMap(data);
	entity->add<CView>(asVector2f(map["size"]), asFloatRect(map["limits"]));
}

template<> void parser<CName>(Template* entity, const ComponentData& data)
{
	entity->add<CName>(as<std::string_view>(data));
}

template<> void parser<CHudRender>(Template* entity, const ComponentData& data)
{
	const auto& map = asLuaMap(data);
	entity->add<CHudRender>(asVector2f(map["pos"]), asVector2f(map["size"]), sf::Color::Black);
}

static constexpr std::initializer_list<std::pair<const std::string_view, ComponentParser::Parser>> parsers =
{
	{"Position", parser<CPosition>},
	{"Render", parser<CRender>},
	{"Move", parser<CMove>},
	{"Input", parser<CInput>},
	{"CollisionBox", parser<CCollisionBox>},
	{"Rigidbody", parser<CRigidbody>},
	{"View", parser<CView>},
	{"Name", parser<CName>},
	{"HUD", parser<CHudRender>},
};
static_assert(parsers.size() == std::tuple_size_v<Components>);
const std::unordered_map<std::string_view, ComponentParser::Parser> ComponentParser::s_m_parsers = parsers;
