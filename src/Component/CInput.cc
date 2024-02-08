#include <Component/CInput.hh>
#include <cassert>

bool CInput::isKeyPressed(const std::string& keyName) const
{
    for (const auto& [key, value] : controls)
    {
        if (value == keyName)
        {
            return sf::Keyboard::isKeyPressed(key);
        }
    }
    assert(false);
}
