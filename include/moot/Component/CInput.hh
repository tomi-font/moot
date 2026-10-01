#pragma once

#include <moot/Input/Binding.hh>
#include <vector>

class CInput
{
public:

	CInput(std::vector<Binding>&& bindings) : m_bindings(std::move(bindings)) {}

	auto& bindings() { return m_bindings; }

	bool bindsKey(sf::Keyboard::Key) const;

private:

	std::vector<Binding> m_bindings;
};
