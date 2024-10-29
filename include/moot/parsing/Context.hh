#pragma once

#include <sol/forward.hpp>

class World;

class ParsingContext
{
public:

	ParsingContext();
	~ParsingContext();

	void initialize(World*);

	void process(const std::string& file);

	void update();

private:

	void registerUniversal();

	sol::state& m_lua;
};
