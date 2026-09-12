#include <moot/parsing/ComponentAttributes.hh>
#include <moot/Component/CCollisionBox.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CInput.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CCamera.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/EntityFunctions.hh>
#include <moot/parsing/types.hh>
#include <SFML/System/Angle.hpp>

template<typename C> static void registerAttributeValues(sol::state* lua);

template<> void registerAttributeValues<CInput>(sol::state* lua)
{
	auto et = lua->create_table("InputEvent");

	et["WindowClose"] = sf::Event(sf::Event::Closed{});
	et["MouseWheelScroll"] = sf::Event(sf::Event::MouseWheelScrolled{.wheel = sf::Mouse::Wheel::Vertical});
	et["MouseMove"] = sf::Event(sf::Event::MouseMoved{});
	et["KeyPress"] = [](sf::Keyboard::Key keyCode) -> sf::Event { return sf::Event::KeyPressed{.code = keyCode}; };
	et["KeyRelease"] = [](sf::Keyboard::Key keyCode) -> sf::Event { return sf::Event::KeyReleased{.code = keyCode}; };
	et["MouseButtonPress"] = [](sf::Mouse::Button button) -> sf::Event { return sf::Event::MouseButtonPressed{.button = button}; };
	et["MouseButtonRelease"] = [](sf::Mouse::Button button) -> sf::Event { return sf::Event::MouseButtonReleased{.button = button}; };

	lua->new_enum("Key",
		"A", sf::Keyboard::Key::A,
		"D", sf::Keyboard::Key::D,
		"Q", sf::Keyboard::Key::Q,
		"S", sf::Keyboard::Key::S,
		"W", sf::Keyboard::Key::W
	);
	lua->new_enum("MouseButton",
		"Left", sf::Mouse::Button::Left,
		"Right", sf::Mouse::Button::Right
	);
}

template<> void registerAttributeValues<Color>(sol::state* lua)
{
	auto colors = lua->create_table("Color");

	colors["None"] = Color();
	colors["Black"] = Color(0, 0, 0);
	colors["Brown"] = Color(101, 67, 33);
	colors["ForestGreen"] = Color(0, 110, 51);
	colors["Gray"] = Color(128, 128, 128);
	colors["SkyBlue"] = Color(135, 206, 235);
	colors["White"] = Color(255, 255, 255);
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
	const auto& heightObj = map["height"];
	const auto& fillColorObj = map["fillColor"];
	assert(mapSize == 1u + heightObj.valid() + fillColorObj.valid());

	std::vector<Vector2f> vertices;
	for (const auto& [_, value] : asLuaArray(map["vertices"]))
		vertices.push_back(asVector2f(value));

	collection->add<CConvexPolygon>(std::move(vertices),
	                                asOptionalParsed<float>(heightObj, 0),
	                                asOptionalParsed<Color>(fillColorObj));
}

template<> void parser<CMove>(const sol::object& data, ComponentCollection* collection)
{
	const auto& map = asLuaMap<1>(data);
	collection->add<CMove>(as<float>(map["speed"]));
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

template<> void parser<CCamera>(const sol::object& data, ComponentCollection* collection)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& sizeObj = map["size"];
	const auto& limitsObj = map["limits"];
	const auto& elevationObj = map["elevation"];
	const auto& rotationObj = map["rotation"];
	assert(int(mapSize) == sizeObj.valid() + limitsObj.valid() + elevationObj.valid() + rotationObj.valid());

	Vector2f size;
	FloatRect limits;
	float elevation = CCamera::MaxElevation;
	float rotation = 0;

	if (sizeObj.valid())
		size = asVector2f(sizeObj);
	if (limitsObj.valid())
		limits = asFloatRect(limitsObj);

	// Angles are given in degrees.
	if (elevationObj.valid())
		elevation = sf::degrees(as<float>(elevationObj)).asRadians();
	if (rotationObj.valid())
		rotation = sf::degrees(as<float>(rotationObj)).asRadians();

	collection->add<CCamera>(size, limits, elevation, rotation);
}

template<> void parser<CHudRender>(const sol::object& data, ComponentCollection* collection)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& sizeObj = map["size"];
	assert(mapSize == 2u + sizeObj.valid());

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

template<> void parser<CLight>(const sol::object& data, ComponentCollection* collection)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	assert(mapSize == 2);
	collection->add<CLight>(asColor(map["emission"]), as<float>(map["radius"]));
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
		registerComponent<CCamera>("Camera");
		registerComponent<CHudRender>("HudRender");
		registerComponent<CPointable>("Pointable");
		registerComponent<CLight>("Light");
	}
} _;
