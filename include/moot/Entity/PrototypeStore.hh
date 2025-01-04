#pragma once

#include <moot/Entity/Prototype.hh>
#include <SFML/System/NonCopyable.hpp>

class PrototypeStore : sf::NonCopyable
{
public:

	using PrototypeId = unsigned;

	std::pair<Prototype*, PrototypeId> newPrototype();
	Prototype* newPrototype(std::string&& name);

	const Prototype& getPrototype(PrototypeId id) const { return m_numberedPrototypes.at(id); }
	const Prototype& getPrototype(const std::string& name) const { return m_namedPrototypes.at(name); }
	const Prototype* findPrototype(const std::string& name) const;

	void deletePrototype(PrototypeId);

private:

	std::unordered_map<PrototypeId, Prototype> m_numberedPrototypes;
	PrototypeId m_nextId = 0;

	std::unordered_map<std::string, Prototype> m_namedPrototypes;
};
