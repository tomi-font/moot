#ifndef COMPONENT_HH
#define COMPONENT_HH

class	Component
{
public:

	virtual ~Component() {}

	enum Type
	{
	};

	Type	getType() const
	{
		return m_type;
	}

private:

	Component(Type);

	Type	m_type;
};

#endif
