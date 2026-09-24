#pragma once

#include <moot/Property/Properties.hh>

class PropertyUser
{
public:

	virtual ~PropertyUser() = default;

	void initializeProperties(Properties*);

protected:

	Properties* m_properties = nullptr;

private:

	virtual void registerProperties();
};
