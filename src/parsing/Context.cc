#include <moot/parsing/Context.hh>
#include <moot/parsing/CallbackParameters.hh>
#include <moot/parsing/ComponentAttributes.hh>
#include <moot/parsing/EntityFunctions.hh>
#include <moot/parsing/GlobalFunctions.hh>

ParsingContext::ParsingContext() : m_lua(new sol::state)
{
	m_lua->open_libraries(sol::lib::base);
}

ParsingContext::~ParsingContext()
{
}

[[noreturn]] static void luaErrorHandler(lua_State* L, std::string_view msg)
{
	lua_Debug ar;
	for (int level = 1; lua_getstack(L, level, &ar); ++level) {
		if (lua_getinfo(L, "Sl", &ar) && ar.currentline > 0) {
			break; // found first Lua frame
		}
	}
	std::cerr << ar.short_src << ':' << ar.currentline << ": " << msg << std::endl;
	assert(false);
}

void ParsingContext::initializeScriptContext(Game* game)
{
	auto errorHandler = (*m_lua)["errorHandler"];
	errorHandler.set_function(
		[lua = m_lua.get()](std::string_view msg)
		{
			luaErrorHandler(lua->lua_state(), msg);
		});
	sol::protected_function::set_default_handler(errorHandler);

	ComponentAttributes::registerAll(m_lua.get());
	GlobalFunctions::registerAll(m_lua.get(), game);
	EntityFunctions::registerAll(m_lua.get());
	CallbackParameters::registerAll(m_lua.get());
}

void ParsingContext::updateScriptContext()
{
	// Force the garbage collection between frames to make sure that no EntityPointer remains.
	m_lua->collect_garbage();
}

void ParsingContext::setScriptSearchPath(std::filesystem::path path)
{
	m_searchPath = std::move(path);
}

void ParsingContext::processScript(const std::filesystem::path& filePath)
{
	m_lua->script_file(m_searchPath / filePath);
}
