#include <moot/Entity/PrototypeStore.hh>

std::pair<Prototype*, PrototypeUid> PrototypeStore::newPrototype()
{
	const PrototypeUid uid = m_nextUid++;
	return { &m_prototypes[uid], uid };
}

const Prototype& PrototypeStore::getPrototype(PrototypeUid uid) const noexcept
{
	return m_prototypes.at(uid);
}

void PrototypeStore::deletePrototype(PrototypeUid uid)
{
	const bool erased = m_prototypes.erase(uid);
	assert(erased);
}
