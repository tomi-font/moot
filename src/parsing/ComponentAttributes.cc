#include <parsing/ComponentAttributes.hh>
#include <Entity/Entity.hh>
#include <parsing/ComponentNames.hh>
#include <parsing/types.hh>

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

template<> void registerAttributeValues<sf::Color>(sol::state* lua)
{
	auto colors = lua->create_table("Color");
	
	colors["Black"] = sf::Color::Black;
}

void ComponentAttributes::registerAll(sol::state* lua)
{
	registerAttributeValues<CInput>(lua);
	registerAttributeValues<sf::Color>(lua);
}

template<typename C> static ComponentVariant parser(const sol::object&);

template<> ComponentVariant parser<CPosition>(const sol::object& data)
{
	return CPosition(asVector2f(data));
}

template<> ComponentVariant parser<CRender>(const sol::object& data)
{
	const auto& map = asLuaMap<2>(data);
	return CRender(asVector2f(map["size"]), asColor(map["color"]));
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
	CView cView;

	if (sizeObj.valid())
		cView.setSize(asVector2f(sizeObj));
	if (limitsObj.valid())
		cView.setLimits(asFloatRect(limitsObj));

	return cView;
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

using ParserPair = std::pair<const std::string_view, ComponentAttributes::Parser>;

template<typename C> static constexpr ParserPair parserPair = {ComponentName<C>, parser<C>};
static constexpr std::initializer_list<ParserPair> parserPairs =
{
	parserPair<CPosition>,
	parserPair<CRender>,
	parserPair<CMove>,
	parserPair<CInput>,
	parserPair<CCollisionBox>,
	parserPair<CRigidbody>,
	parserPair<CView>,
	parserPair<CName>,
	parserPair<CHudRender>,
};
static_assert(parserPairs.size() == ComponentCount - 1); // CCallback is not parsed here.
decltype(ComponentAttributes::s_m_parsers) ComponentAttributes::s_m_parsers = parserPairs;
