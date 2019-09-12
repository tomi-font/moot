#ifndef SPHYSICS_HH
#define SPHYSICS_HH

#include "System.hh"

class	SPhysics : public System
{
public:

	SPhysics();

	void	enforce(float elapsedTime);
};

#endif
