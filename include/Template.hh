#pragma once

#include <Component/Composable.hh>
#include <vector>
#include <memory>

class Template : public ComponentComposable
{
public:

	Template(ComponentComposition comp) noexcept;

private:

	std::vector<std::unique_ptr<Component>>	m_components;
};
