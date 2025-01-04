#include <moot/Component/Collection/Iterator.hh>

bool ComponentCollectionIteratorBase::operator==(ComponentCollectionIteratorBase& other)
{
	while (m_collectionIt != other.m_collectionIt)
	{
		if (m_idx != (*m_collectionIt)->size())
			return false;

		++m_collectionIt;
		m_idx = 0;
	}
	return true;
}
