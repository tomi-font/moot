#include <parsing/Context.hh>
#include <parsing/ComponentAttributes.hh>
#include <parsing/EntityFunctions.hh>
#include <parsing/types.hh>
#include <World.hh>

ParsingContext::ParsingContext() : m_lua(*new sol::state)
{
	m_lua.open_libraries(sol::lib::base);

	registerUniversal();
}

ParsingContext::~ParsingContext()
{
	delete &m_lua;
}

[[noreturn]] static void luaErrorHandler(std::string_view msg)
{
	std::cerr << msg << std::endl;
	assert(false);
}

void ParsingContext::registerUniversal()
{
	auto errorHandler = m_lua["errorHandler"];
	errorHandler.set_function(luaErrorHandler);
	sol::protected_function::set_default_handler(errorHandler);

	ComponentAttributes::registerAll(&m_lua);
}

static const Template& getTemplate(sol::table componentTable, TemplateStore* templateStore)
{
	auto uidObj = componentTable["uid"];
	if (uidObj.valid())
		return templateStore->getTemplate(uidObj.get<unsigned>());

	const auto [temp, uid] = templateStore->newTemplate();

	for (const auto& [key, value] : componentTable)
	{
		auto parser = ComponentAttributes::getParser(key.as<std::string_view>());
		parser(value, temp);
	}
	
	uidObj = uid;
	return *temp;
}

void ParsingContext::registerSpecific(World* world, TemplateStore* templateStore)
{
	m_lua.set_function("spawn", sol::overload(
		[=](sol::table entity)
		{
			world->instantiate(getTemplate(entity, templateStore));
		},
		[=](sol::table entity, sol::table pos)
		{
			world->instantiate(getTemplate(entity, templateStore), asVector2f(pos));
		}
	));
	m_lua.set_function("exitGame", &World::stopRunning, world);
}

void ParsingContext::process(const std::string& file)
{
	m_lua.script_file(file);
}

void ParsingContext::registerPostPopulating()
{
	EntityFunctions::registerAll(&m_lua);
}
