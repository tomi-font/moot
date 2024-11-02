#include <moot/parsing/ComponentAttributes.hh>
#include <moot/Entity/Entity.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/types.hh>
#include <moot/utility/static_warn.hh>

template<typename C> static void registerAttributeValues(sol::state* lua);

template<> void registerAttributeValues<CInput>(sol::state* lua)
{
	// Event Type
	auto et = lua->create_table("Event");

	et["WindowClose"] = sf::Event(sf::Event::Closed);
	et["MouseWheelScroll"] = sf::Event{sf::Event::MouseWheelScrolled, .mouseWheelScroll.wheel = sf::Mouse::Wheel::VerticalWheel};
	et["MouseMove"] = sf::Event(sf::Event::MouseMoved);
	et["KeyPress"] = [](sf::Keyboard::Key keyCode) { return sf::Event{sf::Event::KeyPressed, .key.code = keyCode}; };
	et["KeyRelease"] = [](sf::Keyboard::Key keyCode) { return sf::Event{sf::Event::KeyReleased, .key.code = keyCode}; };
	et["MouseButtonPress"] = [](sf::Mouse::Button button) { return sf::Event{sf::Event::MouseButtonPressed, .mouseButton.button = button}; };
	et["MouseButtonRelease"] = [](sf::Mouse::Button button) { return sf::Event{sf::Event::MouseButtonReleased, .mouseButton.button = button}; };

	lua->new_enum("Key",
		"A", sf::Keyboard::Key::A,
		"D", sf::Keyboard::Key::D,
		"Q", sf::Keyboard::Key::Q,
		"S", sf::Keyboard::Key::S,
		"W", sf::Keyboard::Key::W
	);
	lua->new_enum("MouseButton",
		"Left", sf::Mouse::Button::Left
	);
}

template<> void registerAttributeValues<Color>(sol::state* lua)
{
	auto colors = lua->create_table("Color");
	
	colors["Black"] = Color(sf::Color::Black);
	colors["ForestGreen"] = Color(0, 110, 51);
	colors["Gray"] = Color(128, 128, 128);
	colors["None"] = Color();
}

void ComponentAttributes::registerAll(sol::state* lua)
{
	registerAttributeValues<CInput>(lua);
	registerAttributeValues<Color>(lua);
}

template<typename C> static ComponentVariant parser(const sol::object&);

template<> ComponentVariant parser<CPosition>(const sol::object& data)
{
	return CPosition(asVector2f(data));
}

template<> ComponentVariant parser<CConvexPolygon>(const sol::object& data)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& fillColorObj = map["fillColor"];
	const auto& outlineColorObj = map["outlineColor"];
	assert(mapSize == 1 + fillColorObj.valid() + outlineColorObj.valid());

	std::vector<Vector2f> vertices;
	for (const auto& [_, value] : asLuaArray(map["vertices"]))
		vertices.push_back(asVector2f(value));

	return CConvexPolygon(std::move(vertices), asParsedOr<Color>(fillColorObj), asParsedOr<Color>(outlineColorObj));
}

template<> ComponentVariant parser<CMove>(const sol::object& data)
{
	const auto& map = asLuaMap<1>(data);
	return CMove(as<unsigned short>(map["speed"]));
}

template<> ComponentVariant parser<CInput>(const sol::object& data)
{
	std::vector<CInput::Watch> watches;

	for (const auto& [_, value] : asLuaArray(data))
	{
		const auto& watch = asLuaArray<2>(value);

		watches.emplace_back(asArray<sf::Event>(watch[1]), as<sol::protected_function>(watch[2]));
	}
	return CInput(std::move(watches));
}

template<> ComponentVariant parser<CCollisionBox>(const sol::object& data)
{
	const auto& map = asLuaMap<1>(data);
	return CCollisionBox(asVector2f(map["size"]));
}

template<> ComponentVariant parser<CRigidbody>(const sol::object& data)
{
	asLuaMap<0>(data);
	return CRigidbody();
}

template<> ComponentVariant parser<CView>(const sol::object& data)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& sizeObj = map["size"];
	const auto& limitsObj = map["limits"];
	assert(mapSize == sizeObj.valid() + limitsObj.valid());
	Vector2f size;
	FloatRect limits;

	if (sizeObj.valid())
		size = asVector2f(sizeObj);
	if (limitsObj.valid())
		limits = asFloatRect(limitsObj);

	return CView(size, limits);
}

template<> ComponentVariant parser<CName>(const sol::object& data)
{
	return CName(as<std::string_view>(data));
}

template<> ComponentVariant parser<CHudRender>(const sol::object& data)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& sizeObj = map["size"];
	assert(mapSize == 2 + sizeObj.valid());
	const auto size = sizeObj.valid() ? asVector2f(sizeObj) : sf::Vector2f();
	return CHudRender(asVector2f(map["pos"]), size, asColor(map["color"]));
}

template<> ComponentVariant parser<CPointable>(const sol::object& data)
{
	static const std::unordered_map<std::string_view, CPointable::EventType> s_eventTypes =
	{
		{"onPointerEntered", CPointable::EventType::PointerEntered},
		{"onPointerLeft", CPointable::EventType::PointerLeft},
	};
	CPointable cPointable;
	for (const auto& [key, value] : asLuaMap(data))
	{
		const CPointable::EventType et = s_eventTypes.at(as<std::string_view>(key));
		cPointable.setCallback(et, as<sol::protected_function>(value));
	}
	return std::move(cPointable);
}

using ParserPairType = std::pair<const std::string_view, ComponentAttributes::Parser>;

template<typename C> static constexpr ParserPairType ParserPair = {ComponentName<C>, parser<C>};
static constexpr std::initializer_list<ParserPairType> parserPairs =
{
	ParserPair<CPosition>,
	ParserPair<CConvexPolygon>,
	ParserPair<CMove>,
	ParserPair<CInput>,
	ParserPair<CCollisionBox>,
	ParserPair<CRigidbody>,
	ParserPair<CView>,
	ParserPair<CName>,
	ParserPair<CHudRender>,
	ParserPair<CPointable>,
};
// CEntity is not made directly accessible.
// CCallback is not parsed here.
static_warn(parserPairs.size() == ComponentCount - 2);

decltype(ComponentAttributes::s_m_parsers) ComponentAttributes::s_m_parsers = parserPairs;
