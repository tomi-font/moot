#pragma once

#include <string>

struct CName : std::string
{
	CName(std::string&& name) : std::string(std::move(name)) {}
};
