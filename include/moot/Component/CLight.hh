#pragma once

#include <moot/struct/Color.hh>

class CLight
{
public:

	CLight(Color emission, float radius) : m_emission(emission), m_radius(radius) {}

	auto& emission() const { return m_emission; }
	auto& radius() const { return m_radius; }

private:

	Color m_emission;
	float m_radius;
};
