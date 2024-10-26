#pragma once

#include <moot/Property/Properties.hh>

class PropertyUser
{
public:

	void setProperties(Properties*);

	virtual void initializeProperties();

protected:

	virtual ~PropertyUser() {}

	Properties* m_properties = nullptr;
};
