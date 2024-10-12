#include <moot/System/System.hh>
#include <moot/Entity/Entity.hh>

System::System(unsigned queryCount) :
	m_queries(queryCount)
{
}

System::~System()
{
}

void System::setWindow(Window* window)
{
	assert(!m_window && window);
	m_window = window;
}

void System::match(Archetype* arch)
{
	for (EntityQuery& query : m_queries)
		query.match(arch);
}

void System::initializeEntity(const Entity& entity) const
{
	for (const EntityQuery& query : m_queries)
		query.initializeEntity(entity);
}
