#pragma once

#include <moot/Property/Manager.hh>

class PropertyUser
{
public:

	void setPropertyManager(PropertyManager*);

	virtual void initializeProperties();

protected:

	virtual ~PropertyUser() {}

	void registerPropertyGetter(auto&&... args) { m_propertyManager->registerGetter(std::forward<decltype(args)>(args)...); }

private:

	PropertyManager* m_propertyManager;
};
