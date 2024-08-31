#include <moot/Entity/Factory.hh>

EntityFactory::~EntityFactory()
{
	// The context must be the last thing to be destroyed.
	m_templates.clear();
}

void EntityFactory::populateWorld(World* world)
{
	m_context.registerPrePopulating(world, this);

	m_context.process("world.lua");

	m_context.registerPostPopulating(world);
}
