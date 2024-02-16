#include <Component/CRigidbody.hh>

void CRigidbody::applyForce(float force)
{
    m_velocity += force;
    m_grounded = false;
}

void CRigidbody::ground()
{
    m_velocity = 0;
    m_grounded = true;
}
