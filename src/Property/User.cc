#include <moot/Property/User.hh>
#include <cassert>

void PropertyUser::setProperties(Properties* properties)
{
	assert(!m_properties && properties);
	m_properties = properties;
}

void PropertyUser::initializeProperties()
{
}
