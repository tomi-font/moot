#include <moot/parsing/ComponentAttributes.hh>
#include <moot/Component/CCollisionBox.hh>
#include <moot/Component/CConvexPolygon.hh>
#include <moot/Component/CHudRender.hh>
#include <moot/Component/CInput.hh>
#include <moot/Component/CLight.hh>
#include <moot/Component/CLocalPosition.hh>
#include <moot/Component/CMove.hh>
#include <moot/Component/CPointable.hh>
#include <moot/Component/CPosition.hh>
#include <moot/Component/CRigidbody.hh>
#include <moot/Component/CCamera.hh>
#include <moot/parsing/ComponentNames.hh>
#include <moot/parsing/EntityFunctions.hh>
#include <moot/parsing/types.hh>
#include <variant>
#include <SFML/System/Angle.hpp>

template<typename C> static void registerAttributeValues(sol::state* lua);

static Control::ButtonPair asButtonPair(const sol::object& obj)
{
	const auto& pair = asLuaArray<2>(obj);
	return {as<Control>(pair[1]).asDigital(), as<Control>(pair[2]).asDigital()};
}

static Control asControl(const sol::object& obj)
{
	if (obj.get_type() != sol::type::table)
		return as<Control>(obj);

	const auto& [map, mapSize] = asLuaMapSize(obj);
	const auto& xObj = map["x"];
	const auto& yObj = map["y"];
	
	if (!xObj.valid() && !yObj.valid())
		return {asButtonPair(obj)}; // Axis, Scroll	

	assert(mapSize == 2 && xObj.valid() && yObj.valid());
	return {Control::ButtonQuad{asButtonPair(xObj), asButtonPair(yObj)}}; // Axes
}

static std::vector<Control> asControls(const sol::table& table)
{
	const auto& luaArray = asLuaArray(table);
	std::vector<Control> controls(luaArray.size(), Control::Physical());

	for (const auto& [luaIndex, value] : luaArray)
		controls[as<unsigned>(luaIndex) - 1] = asControl(value);

	return controls;
}

template<> void registerAttributeValues<CInput>(sol::state* lua)
{
	// Registered before any value of theirs goes into a table, so that it gets the real metatable.
	lua->new_usertype<Control>("mt.Control", sol::no_constructor);
	lua->new_usertype<Binding>("mt.Binding", sol::no_constructor);

	const auto bindingFactory = [](Binding::Type type)
	{
		return [type](const sol::table& controls) { return Binding(type, asControls(controls)); };
	};
	lua->set_function("Button", bindingFactory(Binding::Type::Button));
	lua->set_function("Axis", bindingFactory(Binding::Type::Axis));
	lua->set_function("Axes", bindingFactory(Binding::Type::Axes));
	lua->set_function("Motion", bindingFactory(Binding::Type::Motion));
	lua->set_function("Scroll", bindingFactory(Binding::Type::Scroll));
	lua->set("Pointer", Binding(Binding::Type::Pointer, {}));

	using Key = sf::Keyboard::Key;
	auto keys = lua->create_table("Key");
	const auto addKey = [&keys](std::string_view name, Key code) { keys[name] = Control(Control::Key{code}); };

	static_assert(int(Key::Z) - int(Key::A) == 'Z' - 'A');
	for (char letter = 'A'; letter <= 'Z'; ++letter)
		addKey(std::string(1, letter), Key(int(Key::A) + letter - 'A'));

	static_assert(int(Key::Num9) - int(Key::Num0) == 9);
	for (char digit = '0'; digit <= '9'; ++digit)
		addKey("Num" + std::string(1, digit), Key(int(Key::Num0) + digit - '0'));

	addKey("Left", Key::Left);
	addKey("Right", Key::Right);
	addKey("Up", Key::Up);
	addKey("Down", Key::Down);
	addKey("Space", Key::Space);
	addKey("Enter", Key::Enter);
	addKey("Escape", Key::Escape);
	addKey("LShift", Key::LShift);
	addKey("LControl", Key::LControl);

	auto mouse = lua->create_table("Mouse");
	mouse["Left"] = Control(Control::MouseButton{sf::Mouse::Button::Left});
	mouse["Right"] = Control(Control::MouseButton{sf::Mouse::Button::Right});
	mouse["Middle"] = Control(Control::MouseButton{sf::Mouse::Button::Middle});
	mouse["Wheel"] = Control(Control::MouseWheel{});
	mouse["Motion"] = Control(Control::MouseMotion{});

	using PadButton = GamepadLayout::Button;
	using PadStick = GamepadLayout::Stick;
	auto pad = lua->create_table("Pad");
	const auto padButton = [&pad](std::string_view name, PadButton button) { pad[name] = Control(Control::PadButton{button}); };

	padButton("A", PadButton::A);
	padButton("B", PadButton::B);
	padButton("X", PadButton::X);
	padButton("Y", PadButton::Y);
	padButton("LB", PadButton::LB);
	padButton("RB", PadButton::RB);
	padButton("Back", PadButton::Back);
	padButton("Start", PadButton::Start);
	padButton("LS", PadButton::LS);
	padButton("RS", PadButton::RS);

	pad["LeftStick"] = Control(Control::PadStick{PadStick::Left});
	pad["RightStick"] = Control(Control::PadStick{PadStick::Right});
	pad["LeftStickX"] = Control(Control::PadStickAxis{.stick = PadStick::Left, .vertical = false});
	pad["LeftStickY"] = Control(Control::PadStickAxis{.stick = PadStick::Left, .vertical = true});
	pad["RightStickX"] = Control(Control::PadStickAxis{.stick = PadStick::Right, .vertical = false});
	pad["RightStickY"] = Control(Control::PadStickAxis{.stick = PadStick::Right, .vertical = true});
	pad["LeftTrigger"] = Control(Control::PadTrigger{GamepadLayout::Trigger::Left});
	pad["RightTrigger"] = Control(Control::PadTrigger{GamepadLayout::Trigger::Right});
	pad["DPad"] = Control(Control::PadDPad{});
	pad["DPadX"] = Control(Control::PadDPadAxis{.vertical = false});
	pad["DPadY"] = Control(Control::PadDPadAxis{.vertical = true});
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

template<> void parser<CLocalPosition>(const sol::object& data, ComponentCollection* collection)
{
	collection->add<CLocalPosition>(asVector2f(data));
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

// The value reaches the script as what it is: a boolean, a number or a vector.
static Binding::Callback asBindingCallback(const sol::object& functionObj)
{
	return [callback = as<sol::protected_function>(functionObj)](EntityHandle& entity, const Binding::Value& value)
	{
		std::visit([&](const auto& typedValue) { callback(entity, typedValue); }, value);
	};
}

template<> void parser<CInput>(const sol::object& data, ComponentCollection* collection)
{
	const auto& luaArray = asLuaArray(data);
	std::vector<Binding> bindings;
	bindings.reserve(luaArray.size());

	for (const auto& [_, value] : luaArray)
	{
		const auto& pair = asLuaArray<2>(value);

		auto binding = as<Binding>(pair[1]);
		binding.setCallback(asBindingCallback(pair[2]));
		bindings.push_back(std::move(binding));
	}
	collection->add<CInput>(std::move(bindings));
}

template<> void parser<CCollisionBox>(const sol::object& data, ComponentCollection* collection)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& rectObj = map["rect"];
	assert(mapSize == rectObj.valid());

	collection->add<CCollisionBox>(asOptionalParsed<FloatRect>(rectObj));
}

template<> void parser<CRigidbody>(const sol::object& data, ComponentCollection* collection)
{
	asLuaMap<0>(data);
	collection->add<CRigidbody>();
}

template<> void parser<CCamera>(const sol::object& data, ComponentCollection* collection)
{
	const auto& [map, mapSize] = asLuaMapSize(data);
	const auto& limitsObj = map["limits"];
	const auto& elevationObj = map["elevation"];
	const auto& rotationObj = map["rotation"];
	assert(mapSize == 1u + limitsObj.valid() + elevationObj.valid() + rotationObj.valid());

	FloatRect limits;
	float elevation = CCamera::MaxElevation;
	float rotation = 0;

	if (limitsObj.valid())
		limits = asFloatRect(limitsObj);

	// Angles are given in degrees.
	if (elevationObj.valid())
		elevation = sf::degrees(as<float>(elevationObj)).asRadians();
	if (rotationObj.valid())
		rotation = sf::degrees(as<float>(rotationObj)).asRadians();

	collection->add<CCamera>(asVector2f(map["size"]), limits, elevation, rotation);
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
		registerComponent<CLocalPosition>("LocalPosition");
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
