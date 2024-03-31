#include <Factory.hh>
#include <Component/Parser.hh>

Factory::Factory() : m_lua(*new sol::state)
{
	m_lua.open_libraries(sol::lib::base);

	registerUniversal();
}

Factory::~Factory()
{
	m_templates.clear();
	// The Lua state must be the last thing to be destroyed.
	delete &m_lua;
}

void Factory::registerUniversal()
{
	ComponentParser::registerAll(&m_lua);
}

void Factory::registerWorldSpecific(World* world)
{
	m_lua.set_function("spawn",
		[this, world](sol::table entity)
		{
			spawn(world, entity);
		}
	);
	m_lua.set_function("exitGame", &World::stopRunning, world);
}

void Factory::populateWorld(World* world)
{
	registerWorldSpecific(world);

	m_lua.script_file("world.lua");
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

void Factory::spawn(World* world, sol::table componentTable)
{
	const auto uid = componentTable[c_uidKey];
	if (!uid.valid())
	{
		componentTable[c_uidKey] = m_templates.size();
		assert(uid.get<unsigned>() == m_templates.size());

		createTemplate(m_templates.emplace_back(), componentTable);
	}

	world->instantiate(m_templates.at(uid.get<unsigned>()));
}
