#pragma once

#include <Entity/TemplateStore.hh>
#include <parsing/Context.hh>

class EntityFactory : TemplateStore
{
public:

	~EntityFactory();

	void populateWorld(World*);

private:

	ParsingContext m_context;
};
