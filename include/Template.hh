#pragma once

#include <Component/Component.hh>
#include <vector>
#include <memory>

class	Template
{
public:

	Template(ComponentComposition comp) noexcept;

	ComponentComposition	comp() const noexcept { return m_comp; }

private:

	ComponentComposition	m_comp;
	std::vector<std::unique_ptr<Component>>	m_components;
};
