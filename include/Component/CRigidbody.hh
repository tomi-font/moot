#ifndef CRIGIDBODY_HH
#define CRIGIDBODY_HH

class	CRigidbody
{
public:

	static constexpr auto Type = Component::Rigidbody;

	float	getVelocity() const noexcept { return m_velocity; }
	void	applyForce(float force) noexcept { m_velocity += force; };
	void	setVelocity(float velocity) noexcept { m_velocity = velocity; }
	void	resetVelocity() noexcept { m_velocity = 0.f; }

	bool	isGrounded() const noexcept { return m_grounded; }
	void	setGrounded(bool grounded) noexcept { m_grounded = grounded; }

private:

	float	m_velocity = 0.f;
	bool	m_grounded = false;
};

#endif
