#include <moot/Entity/PrototypeStore.hh>

std::pair<Prototype*, PrototypeStore::PrototypeId> PrototypeStore::newPrototype()
{
	const PrototypeId id = m_nextId++;
	return { &m_numberedPrototypes[id], id };
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

void PrototypeStore::deletePrototype(PrototypeId id)
{
	const bool erased = m_numberedPrototypes.erase(id);
	assert(erased);
}
