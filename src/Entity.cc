#include <Entity.hh>

Entity::~Entity()
{
	for (Component* c : m_components)
		delete c;
}
