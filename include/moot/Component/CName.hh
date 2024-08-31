#pragma once

#include <string>

struct CName
{
	CName(std::string_view name) : m_name(name) {}

	bool operator==(std::string_view other) const { return m_name == other; }

private:

	std::string m_name;
};
