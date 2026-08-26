#include <moot/Component/CCallback.hh>
#include <cassert>

void CCallback::add(Type type, Callback&& callback)
{
	const bool added = m_callbacks.emplace(type, std::move(callback)).second;
	assert(added);
}

const CCallback::Callback* CCallback::get(Type type) const
{
	if (const auto it = m_callbacks.find(type); it != m_callbacks.end())
		return &it->second;
	
	return nullptr;
}

CCallback::Callback CCallback::extract(Type type)
{
	if (m_callbacks.contains(type))
		return std::move(m_callbacks.extract(type).mapped());

	return {};
}
