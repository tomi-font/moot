#include <Entity/TemplateStore.hh>

std::pair<Template*, unsigned> TemplateStore::newTemplate()
{
	const auto index = m_templates.size();
	return { &m_templates.emplace_back(), index };
}

const Template& TemplateStore::getTemplate(unsigned uid) const
{
	return m_templates[uid];
}
