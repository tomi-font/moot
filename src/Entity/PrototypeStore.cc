#include <moot/Entity/PrototypeStore.hh>

std::pair<Prototype*, PrototypeUid> PrototypeStore::newPrototype()
{
	const PrototypeUid uid = m_nextUid++;
	return { &m_numberedPrototypes[uid], uid };
}

Prototype* PrototypeStore::newPrototype(std::string&& name)
{
	const auto& insertionPair = m_namedPrototypes.try_emplace(std::move(name));
	assert(insertionPair.second);
	return &insertionPair.first->second;
}

const Prototype* PrototypeStore::findPrototype(const std::string& name) const
{
	const auto prototypeIt = m_namedPrototypes.find(name);
	return (prototypeIt == m_namedPrototypes.end()) ? nullptr : &prototypeIt->second;
}

void PrototypeStore::deletePrototype(PrototypeUid uid)
{
	const bool erased = m_numberedPrototypes.erase(uid);
	assert(erased);
}
