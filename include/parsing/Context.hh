#pragma once

#include <Entity/TemplateStore.hh>
#include <sol/forward.hpp>

class World;

class ParsingContext
{
public:

	ParsingContext();
	~ParsingContext();

	void process(const std::string& file);

	void registerSpecific(World*, TemplateStore*);
	void registerPostPopulating();

private:

	void registerUniversal();

	sol::state& m_lua;
};
