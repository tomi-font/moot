#include <moot/Entity/Handle.hh>

void EntityHandle::remove(ComponentId cId)
{
	m_comp -= cId;
	manager->removeComponentFrom(*this, cId);
}
