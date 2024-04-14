#pragma once

#include <Entity/Template.hh>

class TemplateStore
{
public:

	std::pair<Template*, unsigned> newTemplate();
	const Template& getTemplate(unsigned uid) const;

protected:

	std::vector<Template> m_templates;
};
