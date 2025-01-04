#pragma once

template<typename T> class InstanceCounter
{
#ifndef NDEBUG
	static inline unsigned s_m_instanceCount = 0;

public:

	static auto instanceCount() { return s_m_instanceCount; }

protected:

	InstanceCounter() { ++s_m_instanceCount; }
	~InstanceCounter() { --s_m_instanceCount; }

	InstanceCounter(const InstanceCounter&) : InstanceCounter() {}
	InstanceCounter& operator=(const InstanceCounter&) = default;
#endif
};
