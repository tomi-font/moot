#include <moot/Property/User.hh>
#include <cassert>

void PropertyUser::setPropertyManager(PropertyManager* pm)
{
	assert(!m_propertyManager && pm);
	m_propertyManager = pm;
}

void PropertyUser::initializeProperties()
{
}
