#ifndef TEMPLATE_HH
#define TEMPLATE_HH

#include "Component/Component.hh"

// Entity Template
class	Template
{
public:

	Template(CsComp comp) noexcept;

	CsComp	getComp() const noexcept { return m_comp; }

private:

	CsComp	m_comp;
	void*	m_components;
};

#endif
