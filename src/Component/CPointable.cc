#include <moot/Component/CPointable.hh>

void CPointable::notify(EventType eventType, const Entity& entity) const
{
	const auto findIt = m_callbacks.find(eventType);
	if (findIt != m_callbacks.end())
		findIt->second(entity);
}
