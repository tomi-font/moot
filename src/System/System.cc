#include <System/System.hh>
#include <Entity/Entity.hh>


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
	for (ComponentGroup& group : m_groups)
		group.match(arch);
}

void System::processInstantiatedEntity(const Entity& entity) const
{
	for (const ComponentGroup& group : m_groups)
		if (group.initializesEntities() && group.matches(entity.comp()))
			processInstantiatedEntity(entity, static_cast<unsigned>(&group - &m_groups[0]));
}

void System::processInstantiatedEntity(const Entity&, unsigned) const
{
	assert(false);
}
