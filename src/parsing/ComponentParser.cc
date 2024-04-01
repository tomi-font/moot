#include <parsing/ComponentParser.hh>
#include <Entity/Entity.hh>
#include <parsing/types.hh>

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

template<typename T> static void parser(const sol::object&, Template*);

template<> void parser<CPosition>(const sol::object& data, Template* entity)
{
	entity->add<CPosition>(asVector2f(data));
}

template<> void parser<CRender>(const sol::object& data, Template* entity)
{
	const auto& map = asLuaMap<1>(data);
	entity->add<CRender>(asVector2f(map["size"]), sf::Color::Black);
}

template<> void parser<CMove>(const sol::object& data, Template* entity)
{
	const auto& map = asLuaMap<1>(data);
	entity->add<CMove>(as<unsigned>(map["speed"]));
}

template<> void parser<CInput>(const sol::object& data, Template* entity)
{
	std::vector<CInput::Watch> watches;

	for (const auto& [_, value] : asLuaArray(data))
	{
		const auto& watch = asLuaArray<2>(value);

		watches.emplace_back(asArray<sf::Event>(watch[1]), as<sol::function>(watch[2]));
	}
	entity->add<CInput>(std::move(watches));
}

template<> void parser<CCollisionBox>(const sol::object& data, Template* entity)
{
	const auto& map = asLuaMap<1>(data);
	entity->add<CCollisionBox>(asVector2f(map["size"]));
}

template<> void parser<CRigidbody>(const sol::object& data, Template* entity)
{
	asLuaMap<0>(data);
	entity->add<CRigidbody>();
}

template<> void parser<CView>(const sol::object& data, Template* entity)
{
	const auto& map = asLuaMap<2>(data);
	entity->add<CView>(asVector2f(map["size"]), asFloatRect(map["limits"]));
}

template<> void parser<CName>(const sol::object& data, Template* entity)
{
	entity->add<CName>(as<std::string_view>(data));
}

template<> void parser<CHudRender>(const sol::object& data, Template* entity)
{
	const auto& map = asLuaMap<2>(data);
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
