#ifndef COMPONENT_HH
#define COMPONENT_HH

// describes which components are present with each bit representing a different one
using t_Comp = unsigned;
// used to compose the bitmask by OR-ing with T being a Component::Type
#define C(T) (1 << T)

class	Component
{
public:

	enum	Type
	{
		Position,
		Render,
		Move
	};
};

#endif
