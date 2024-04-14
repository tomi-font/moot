#include <parsing/ComponentAttributes.hh>
#include <Entity/Entity.hh>
#include <parsing/types.hh>

static void registerEvents(sol::state* lua)
{
	// Event Type
	auto et = lua->create_table("Event");

	et["WindowClose"] = sf::Event(sf::Event::Closed);
	et["MouseWheelScroll"] = sf::Event{sf::Event::MouseWheelScrolled, .mouseWheelScroll.wheel = sf::Mouse::Wheel::VerticalWheel};
	et["KeyPress"] = [](sf::Keyboard::Key keyCode)
	{
		return sf::Event{sf::Event::KeyPressed, .key.code = keyCode};
	};
	et["KeyRelease"] = [](sf::Keyboard::Key keyCode)
	{
		return sf::Event{sf::Event::KeyReleased, .key.code = keyCode};
	};

	lua->new_usertype<sf::Event>("sf.Event",
		"key", &sf::Event::key,
		"mouseWheel", &sf::Event::mouseWheelScroll);

	lua->new_usertype<sf::Event::KeyEvent>("sf.Event.KeyEvent",
		"code", &sf::Event::KeyEvent::code);
	lua->new_usertype<sf::Event::MouseWheelScrollEvent>("sf.Event.MouseWheelScrollEvent",
		"delta", &sf::Event::MouseWheelScrollEvent::delta);
}

static void registerCInput(sol::state* lua)
{
	registerEvents(lua);

	lua->new_enum("Key",
		"A", sf::Keyboard::Key::A,
		"D", sf::Keyboard::Key::D,
		"Q", sf::Keyboard::Key::Q,
		"S", sf::Keyboard::Key::S,
		"W", sf::Keyboard::Key::W
	);

	// TODO: move to post-populating
	lua->set_function("isKeyPressed",
		[](sf::Keyboard::Key key)
		{
			return static_cast<int>(sf::Keyboard::isKeyPressed(key));
		});
}

void ComponentAttributes::registerAll(sol::state* lua)
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
	const auto& map = asLuaMap<2>(data);
	
	const auto& colorComponents = asArray<sf::Uint8, 3>(map["color"]);
	const sf::Color color = {colorComponents[0], colorComponents[1], colorComponents[2]};

	entity->add<CRender>(asVector2f(map["size"]), color);
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

		watches.emplace_back(asArray<sf::Event>(watch[1]), as<sol::protected_function>(watch[2]));
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

static constexpr std::initializer_list<std::pair<const std::string_view, ComponentAttributes::Parser>> parsers =
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
const std::unordered_map<std::string_view, ComponentAttributes::Parser> ComponentAttributes::s_m_parsers = parsers;
