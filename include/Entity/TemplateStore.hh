#pragma once

#include <Entity/Template.hh>

using TemplateUid = unsigned;

class TemplateStore
{
public:

	std::pair<Template*, TemplateUid> newTemplate();

	const Template& getTemplate(TemplateUid) const noexcept;

	void deleteTemplate(TemplateUid);

protected:

	std::unordered_map<TemplateUid, Template> m_templates;
	TemplateUid m_nextUid = 0;
};
