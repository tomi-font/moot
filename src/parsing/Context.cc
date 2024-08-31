#include <moot/parsing/Context.hh>
#include <moot/parsing/CallbackParameters.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/EntityFunctions.hh>
#include <moot/parsing/GlobalFunctions.hh>

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

void ParsingContext::registerPrePopulating(World* world, TemplateStore* templateStore)
{
	GlobalFunctions::registerPrePopulating(&m_lua, world, templateStore);
}

void ParsingContext::process(const std::string& file)
{
	m_lua.script_file(file);
}

void ParsingContext::registerPostPopulating(World* world)
{
	GlobalFunctions::registerPostPopulating(&m_lua, world);
	EntityFunctions::registerAll(&m_lua);
	CallbackParameters::registerAll(&m_lua);
}

void ParsingContext::update()
{
	// Force the garbage collection between frames to make sure that Entity references do not remain.
	m_lua.collect_garbage();
}
