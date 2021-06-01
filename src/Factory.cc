#include <Factory.hh>
#include <World.hh>
#include <Component/Component.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CRigidbody.hh>

#ifndef RELEASE
#include <iostream>
#endif

bool	Factory::loadTemplate(const std::string& name, TemplateMap::iterator* tempIter)
{
#ifndef RELEASE
	std::cerr << "didn't find template for " << name << std::endl;
	return false;
#endif
}
void	Factory::create(const std::string& name, World* world)
{
	TemplateMap::iterator	tempIter = m_templates.find(name);
	const Template*			temp;
	Archetype*				arch;

	if (tempIter == m_templates.end())
	{
		if (!loadTemplate(name, &tempIter))
			return;
	}
	temp = &tempIter->second;
	arch = world->getArchetype(temp->getComp());
}
