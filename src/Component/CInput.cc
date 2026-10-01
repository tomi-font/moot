#include <moot/Component/CInput.hh>

bool CInput::bindsKey(sf::Keyboard::Key key) const
{
	for (const Binding& binding : m_bindings)
		if (binding.hasKey(key))
			return true;
	return false;
}
