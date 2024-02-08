#pragma once

#include <Component/Composable.hh>
#include <Component/Types.hh>
#include <utility/tuple/toVariant.hh>
#include <set>

class Template : public ComponentComposable
{
public:

	// Variant containing every component.
	using ComponentVariant = tupleToVariant<Components>::type;

	struct DuplicateComponentException{};

	// Adds a component. Throws DuplicateComponentException if trying to add a component that's already present.
	void add(ComponentVariant&&);

	auto& components() const { return m_components; }

private:

	// Custom compare for the component container to sort components by their index in ascending order.
	struct CVCompare { constexpr bool operator()(const ComponentVariant& l, const ComponentVariant& r) const { return l.index() < r.index(); } };

	// A set allows easily inserting components in their sorting order without knowing in advance the final composition.
	std::set<ComponentVariant, CVCompare> m_components;
};
