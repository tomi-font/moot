#pragma once

#include <sol/forward.hpp>

class TemplateStore;
class World;

class ParsingContext
{
public:

	ParsingContext();
	~ParsingContext();

	void registerPrePopulating(World*, TemplateStore*);
	void registerPostPopulating(World*);
	
	void process(const std::string& file);

	void update();

private:

	void registerUniversal();

	sol::state& m_lua;
};
