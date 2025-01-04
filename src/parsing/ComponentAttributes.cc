#include <moot/parsing/ComponentAttributes.hh>
#include <moot/Component/CCollisionBox.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CInput.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CView.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/EntityFunctions.hh>
#include <moot/parsing/types.hh>

template<typename C> static void registerAttributeValues(sol::state* lua);

template<> void registerAttributeValues<CInput>(sol::state* lua)
{
	auto et = lua->create_table("InputEvent");

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

template<typename C> static void parser(const sol::object&, ComponentCollection*);

template<> void parser<CPosition>(const sol::object& data, ComponentCollection* collection)
{
	collection->add<CPosition>(asVector2f(data));
}

template<> void parser<CConvexPolygon>(const sol::object& data, ComponentCollection* collection)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& fillColorObj = map["fillColor"];
	const auto& outlineColorObj = map["outlineColor"];
	assert(mapSize == 1 + fillColorObj.valid() + outlineColorObj.valid());

	std::vector<Vector2f> vertices;
	for (const auto& [_, value] : asLuaArray(map["vertices"]))
		vertices.push_back(asVector2f(value));

	collection->add<CConvexPolygon>(std::move(vertices), asParsedOr<Color>(fillColorObj), asParsedOr<Color>(outlineColorObj));
}

template<> void parser<CMove>(const sol::object& data, ComponentCollection* collection)
{
	const auto& map = asLuaMap<1>(data);
	collection->add<CMove>(as<unsigned short>(map["speed"]));
}

template<> void parser<CInput>(const sol::object& data, ComponentCollection* collection)
{
	std::vector<CInput::Watch> watches;

	for (const auto& [_, value] : asLuaArray(data))
	{
		const auto& watch = asLuaArray<2>(value);

		watches.emplace_back(asArray<sf::Event>(watch[1]), as<sol::protected_function>(watch[2]));
	}
	collection->add<CInput>(std::move(watches));
}

template<> void parser<CCollisionBox>(const sol::object& data, ComponentCollection* collection)
{
	const auto& map = asLuaMap<1>(data);
	collection->add<CCollisionBox>(asVector2f(map["size"]));
}

template<> void parser<CRigidbody>(const sol::object& data, ComponentCollection* collection)
{
	asLuaMap<0>(data);
	collection->add<CRigidbody>();
}

template<> void parser<CView>(const sol::object& data, ComponentCollection* collection)
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

	collection->add<CView>(size, limits);
}

template<> void parser<CHudRender>(const sol::object& data, ComponentCollection* collection)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& sizeObj = map["size"];
	assert(mapSize == 2 + sizeObj.valid());
	const auto size = sizeObj.valid() ? asVector2f(sizeObj) : sf::Vector2f();
	collection->add<CHudRender>(asVector2f(map["pos"]), size, asColor(map["color"]));
}

template<> void parser<CPointable>(const sol::object& data, ComponentCollection* collection)
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
	collection->add<CPointable>(std::move(cPointable));
}

decltype(ComponentAttributes::s_m_parsers) ComponentAttributes::s_m_parsers;

void ComponentAttributes::registerParser(ComponentId cId, Parser parser)
{
	const auto& name = ComponentNames::get(cId);
	assert(!s_m_parsers.contains(name));
	s_m_parsers[name] = parser;
}

template<typename C> static sol::object componentGetter(const EntityHandle& entity, lua_State* luaState)
{
	return sol::make_object(luaState, entity.get<C*>());
}

template<> sol::object componentGetter<CPosition>(const EntityHandle& entity, lua_State* luaState)
{
	return sol::make_object(luaState, &entity.get<CPosition*>()->mut());
}

template<typename C> static void registerComponent(std::string name)
{
	ComponentNames::add<C>(std::move(name));
	ComponentAttributes::registerParser<C>(parser<C>);
	EntityFunctions::registerComponentGetter<C>(componentGetter<C>);
}

static struct Init
{
	Init()
	{
		registerComponent<CPosition>("Position");
		registerComponent<CConvexPolygon>("ConvexPolygon");
		registerComponent<CMove>("Move");
		registerComponent<CInput>("Input");
		registerComponent<CCollisionBox>("CollisionBox");
		registerComponent<CRigidbody>("Rigidbody");
		registerComponent<CView>("View");
		registerComponent<CHudRender>("HudRender");
		registerComponent<CPointable>("Pointable");
	}
} _;
