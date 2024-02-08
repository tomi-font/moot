#pragma once

#include <functional>
#include <string>
#include <SFML/Window/Keyboard.hpp>

class EntityHandle;

struct CInput
{
	const std::unordered_map<sf::Keyboard::Key, std::string> controls;
	
	const std::function<void (EntityHandle, const std::string& key)> callback;

	bool isKeyPressed(const std::string& keyName) const;
};
