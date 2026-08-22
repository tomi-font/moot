#pragma once

#include <filesystem>
#include <memory>

namespace sol { class state; }
class Game;

class ParsingContext
{
public:

	void setScriptSearchPath(std::filesystem::path);
	void processScript(const std::filesystem::path&);

	auto* scriptContext() { return m_lua.get(); }

protected:

	ParsingContext();
	~ParsingContext();

	void initializeScriptContext(Game*);

	void updateScriptContext();

private:

	std::unique_ptr<sol::state> m_lua;

	std::filesystem::path m_searchPath;
};
