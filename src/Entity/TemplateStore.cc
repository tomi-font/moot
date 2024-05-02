#include <Entity/TemplateStore.hh>

std::pair<Template*, TemplateUid> TemplateStore::newTemplate()
{
	const TemplateUid uid = m_nextUid++;
	return { &m_templates[uid], uid };
}

const Template& TemplateStore::getTemplate(TemplateUid uid) const noexcept
{
	return m_templates.at(uid);
}

void TemplateStore::deleteTemplate(TemplateUid uid)
{
	const bool erased = m_templates.erase(uid);
	assert(erased);
}
