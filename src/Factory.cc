#include <Factory.hh>
#include <Component/Parser.hh>

Factory::Factory() : m_lua(new sol::state)
{
	auto& lua = *m_lua;

	lua.open_libraries(sol::lib::base);

	lua.set_function("spawn", &Factory::spawn, this);
}

Factory::~Factory()
{
}

void Factory::populateWorld(World* world)
{
	m_world = world;

	m_lua->script_file("world.lua");
}

constexpr std::string_view c_uidKey = "uid";

static void createTemplate(Template& entity, sol::table& componentTable)
{
	for (const auto& [key, value] : componentTable)
	{
		const auto& componentName = key.as<std::string_view>();
		if (componentName == c_uidKey)
			continue;

		auto parser = ComponentParser::get(key.as<std::string_view>());
		parser(&entity, value);
	}
}

void Factory::spawn(sol::table componentTable)
{
	const auto uid = componentTable[c_uidKey];
	if (!uid.valid())
	{
		componentTable[c_uidKey] = m_templates.size();
		assert(uid.get<unsigned>() == m_templates.size());

		createTemplate(m_templates.emplace_back(), componentTable);
	}

	m_world->instantiate(m_templates.at(uid.get<unsigned>()));
}
