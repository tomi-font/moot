#pragma once

#include <Component/Composable.hh>
#include <Component/Types.hh>
#include <variant>
#include <set>

class Template : public ComponentComposable
{
// Helper struct to convert a tuple into a variant.
	template<typename Tuple> struct VariantGetter;
	template<typename... Ts> struct VariantGetter<std::tuple<Ts...>> { using type = std::variant<Ts...>; };
// Variant containing every component.
	using ComponentVariant = typename VariantGetter<Components>::type;

public:

	class DuplicateComponentException{};

// Adds a component. May throw DuplicateComponentException if trying to add a component that's already present.
	void add(const ComponentVariant&);

private:

// Custom Compare for the components' container. Components are unique so all we need to compare is their type (variant index).
	struct CVCompare { constexpr bool	operator()(const ComponentVariant& l, const ComponentVariant& r) const { return l.index() < r.index(); } };

	std::set<ComponentVariant, CVCompare>	m_components;
};
