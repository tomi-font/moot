#include <Factory.hh>
#include <World.hh>
#include <Component/Component.hh>
#include <Component/CPosition.hh>
#include <Component/CRender.hh>
#include <Component/CMove.hh>
#include <Component/CPlayer.hh>
#include <Component/CCollisionBox.hh>
#include <Component/CRigidbody.hh>
#include <fstream>

#ifndef RELEASE
#include <iostream>
#endif

#define TEMPLATES_DIR "data/templates/"

bool	Factory::loadTemplate(const std::string& name, TemplateMap::iterator* tempIter)
{
	char*	content;
// first reading the whole file into content
	{
		std::ifstream	ifs(TEMPLATES_DIR + name, std::ios::ate);

		if (ifs.fail())
		{
		#ifndef RELEASE
			std::cerr << "didn't find template for " << name << std::endl;
		#endif
			return false;
		}

		std::streamsize	len = ifs.tellg();
		if (len == std::numeric_limits<std::streamsize>::max())
			return false;
		content = new char[len + 1];
		content[len] = '\0';
		ifs.seekg(0);
		ifs.read(content, len);
		ifs.close();

		if (ifs.fail())
			return false;
	}

	return true;
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
