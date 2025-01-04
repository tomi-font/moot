#include <moot/Component/CPointable.hh>

void CPointable::notify(EventType eventType, EntityHandle& entity) const
{
	const auto callbackIt = m_callbacks.find(eventType);
	if (callbackIt != m_callbacks.end())
		callbackIt->second(entity);
}
