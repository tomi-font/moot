#pragma once

struct CRigidbody
{
public:

	void applyForce(float force) { velocity += force; }

	float velocity = 0.f;

	// TODO: Re-think the grounded mechanism.
	bool grounded = false;
};
