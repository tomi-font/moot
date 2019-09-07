#ifndef ENTITY_HH
#define ENTITY_HH

#include "Component/Component.hh"
#include <SFML/System/Vector2.hpp>
#include <vector>

class	Entity
{
public:

	Entity(const sf::Vector2f& pos) : m_pos(pos) {}
	~Entity();

	const sf::Vector2f&	getPosition() const
	{
		return m_pos;
	}

	void	addComponent(Component* c)
	{
		m_components.push_back(c);
	}

	template<class T> T* getComponent() const
	{
		for (Component* c : m_components)
		{
			if (c->getType() == T::s_type)
				return static_cast<T*>(c);
		}
		return nullptr;
	};

private:
	
	sf::Vector2f			m_pos;
	std::vector<Component*>	m_components;
};

#endif
