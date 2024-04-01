#include <Factory.hh>
#include <parsing/ComponentParser.hh>
#include <parsing/types.hh>

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
	m_lua.set_function("spawn", sol::overload(
		[this, world](sol::table entity)
		{
			world->instantiate(getTemplate(entity));
		},
		[this, world](sol::table entity, sol::table pos)
		{
			world->instantiate(getTemplate(entity), asVector2f(pos));
		}
	));
	m_lua.set_function("exitGame", &World::stopRunning, world);
}

void Factory::populateWorld(World* world)
{
	registerWorldSpecific(world);

	m_lua.script_file("world.lua");
}

static void parseTemplate(Template& entity, const sol::table& componentTable)
{
	for (const auto& [key, value] : componentTable)
	{
		ComponentParser::Parser parser = ComponentParser::get(key.as<std::string_view>());
		parser(value, &entity);
	}
}

const Template& Factory::getTemplate(sol::table componentTable)
{
	constexpr std::string_view c_uidKey = "uid";
	unsigned index;
	const auto uid = componentTable[c_uidKey];
	if (!uid.valid())
	{
		index = static_cast<unsigned>(m_templates.size());
		parseTemplate(m_templates.emplace_back(), componentTable);

		componentTable[c_uidKey] = index;
		assert(uid.get<unsigned>() == index);
	}
	else
	{
		// TODO: If in Lua tables are copied after having been spawned the UID will likely also get copied.
		index = uid.get<unsigned>();
		assert(index < m_templates.size());
	}
	return m_templates[index];
}
