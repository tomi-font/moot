#pragma once

class CRigidbody
{
public:

	void applyForce(float force);
	void ground();

	float velocity() const { return m_velocity; }
	bool grounded() const { return m_grounded; }

private:

	float m_velocity = 0;

	// TODO: Re-think the grounded mechanism.
	bool m_grounded = false;
};
