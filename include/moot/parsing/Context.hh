#pragma once

#include <filesystem>

namespace sol { class state; }
class World;

class ParsingContext
{
public:

	void setScriptSearchPath(std::filesystem::path);

	void processScript(const std::filesystem::path&);

protected:

	ParsingContext();
	~ParsingContext();

	void initializeScriptContext(World*);

	void updateScriptContext();

private:

	sol::state& m_lua;

	std::filesystem::path m_searchPath;
};
