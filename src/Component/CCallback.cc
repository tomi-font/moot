#include <moot/Component/CCallback.hh>
#include <cassert>

void CCallback::add(Type type, Callback&& callback)
{
	const bool added = m_callbacks.emplace(type, std::move(callback)).second;
	assert(added);
}

CCallback::Callback CCallback::extract(Type type)
{
	if (m_callbacks.contains(type))
		return std::move(m_callbacks.extract(type).mapped());

	return {};
}
