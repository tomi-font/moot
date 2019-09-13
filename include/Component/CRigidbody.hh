#ifndef CRIGIDBODY_HH
#define CRIGIDBODY_HH

class	CRigidbody
{
public:

	float	getVelocity() const noexcept { return m_velocity; }
	void	applyForce(float force) noexcept { m_velocity += force; };
	void	resetVelocity() noexcept { m_velocity = 0.f; }

	bool	isGrounded() const noexcept { return m_grounded; }
	void	setGrounded(bool grounded) noexcept { m_grounded = grounded; }

private:

	float	m_velocity = 0.f;
	bool	m_grounded = false;
};

#endif
