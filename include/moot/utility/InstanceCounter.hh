#pragma once

template<typename T> class InstanceCounter
{
	static inline unsigned s_m_instanceCount = 0;

protected:

	static auto instanceCount() { return s_m_instanceCount; }

	InstanceCounter() { ++s_m_instanceCount; }
	~InstanceCounter() { --s_m_instanceCount; }

	InstanceCounter(const InstanceCounter&) : InstanceCounter() {}
	InstanceCounter& operator=(const InstanceCounter&) = default;
};
