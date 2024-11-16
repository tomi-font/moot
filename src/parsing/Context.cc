#include <moot/parsing/Context.hh>
#include <moot/parsing/CallbackParameters.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/EntityFunctions.hh>
#include <moot/parsing/GlobalFunctions.hh>

ParsingContext::ParsingContext() : m_lua(*new sol::state)
{
	m_lua.open_libraries(sol::lib::base);
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

void ParsingContext::initializeScriptContext(Game* game)
{
	auto errorHandler = m_lua["errorHandler"];
	errorHandler.set_function(luaErrorHandler);
	sol::protected_function::set_default_handler(errorHandler);

	ComponentAttributes::registerAll(&m_lua);
	GlobalFunctions::registerAll(&m_lua, game);
	EntityFunctions::registerAll(&m_lua);
	CallbackParameters::registerAll(&m_lua);
}

void ParsingContext::updateScriptContext()
{
	// Force the garbage collection between frames to make sure that Entity references do not remain.
	m_lua.collect_garbage();
}

void ParsingContext::setScriptSearchPath(std::filesystem::path path)
{
	m_searchPath = std::move(path);
}

void ParsingContext::processScript(const std::filesystem::path& filePath)
{
	m_lua.script_file(m_searchPath / filePath);
}
