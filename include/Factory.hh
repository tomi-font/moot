#ifndef FACTORY_HH
#define FACTORY_HH

#include "Template.hh"
#include <unordered_map>

class	World;

// Entity Factory
class	Factory
{
public:

	void	create(const std::string& entityName, World* world);

private:

	using TemplateMap = std::unordered_map<std::string, Template>;
	TemplateMap	m_templates;

	bool	loadTemplate(const std::string&, TemplateMap::iterator*);
};

#endif
