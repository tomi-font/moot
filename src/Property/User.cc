#include <moot/Property/User.hh>
#include <cassert>

void PropertyUser::initializeProperties(Properties* properties)
{
	assert(!m_properties && properties);
	m_properties = properties;
	registerProperties();
}

void PropertyUser::registerProperties()
{
}
