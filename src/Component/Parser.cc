#include <Component/Parser.hh>

using ComponentData = ComponentParser::Data;

template<typename T> T as(const ComponentData& var)
{
	assert(var.valid());
	assert(var.get_type() == sol::lua_type_of_v<T>);
	return var.as<T>();
}

static sol::table asTable(const ComponentData& var)
{
	return as<sol::table>(var);
}

template<typename T, unsigned N> static std::array<T, N> asArray(const ComponentData& var)
{
	std::array<T, N> array;

	const auto& table = asTable(var);
	assert(table.size() == N);

	for (unsigned i = 0; i != N; ++i)
		array[i] = table[i + 1].get<T>();
	return array;
}

template<typename T> sf::Vector2<T> asVector2(const ComponentData& var)
{
	const auto& array = asArray<T, 2>(var);
	return {array[0], array[1]};
}
static auto asVector2f(const ComponentData& var)
{
	return asVector2<float>(var);
}

template<typename T> Rect<T> asRect(const ComponentData& var)
{
	const auto& array = asArray<T, 4>(var);
	return {array[0], array[1], array[2], array[3]};
}
static auto asFloatRect(const ComponentData& var)
{
	return asRect<float>(var); 
}

template<typename T> void parser(Template*, const ComponentData&);

template<> void parser<CPosition>(Template* entity, const ComponentData& data)
{
	entity->add<CPosition>(asVector2f(data));
}

template<> void parser<CRender>(Template* entity, const ComponentData& data)
{
	const auto& table = asTable(data);
	entity->add<CRender>(asVector2f(table["pos"]), asVector2f(table["size"]), sf::Color::Black);
}

template<> void parser<CMove>(Template* entity, const ComponentData& data)
{
	const auto& table = asTable(data);
	entity->add<CMove>(as<unsigned>(table["speed"]));
}

template<> void parser<CInput>(Template* entity, const ComponentData& data)
{
}

template<> void parser<CCollisionBox>(Template* entity, const ComponentData& data)
{
	const auto& table = asTable(data);
	entity->add<CCollisionBox>(asVector2f(table["pos"]), asVector2f(table["size"]));
}

template<> void parser<CRigidbody>(Template* entity, const ComponentData& data)
{
	assert(asTable(data).empty());
	entity->add<CRigidbody>();
}

template<> void parser<CView>(Template* entity, const ComponentData& data)
{
	const auto& table = asTable(data);
	entity->add<CView>(asVector2f(table["size"]), asFloatRect(table["limits"]));
}

template<> void parser<CName>(Template* entity, const ComponentData& data)
{
	entity->add<CName>(as<std::string>(data));
}

template<> void parser<CHudRender>(Template* entity, const ComponentData& data)
{
	const auto& table = asTable(data);
	entity->add<CHudRender>(asVector2f(table["pos"]), asVector2f(table["size"]), sf::Color::Black);
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
