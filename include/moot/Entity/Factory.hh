#pragma once

#include <moot/Entity/TemplateStore.hh>
#include <moot/parsing/Context.hh>

class EntityFactory : TemplateStore
{
public:

	~EntityFactory();

	void populateWorld(World*);

	void update() { m_context.update(); }

private:

	ParsingContext m_context;
};
