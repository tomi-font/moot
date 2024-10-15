#pragma once

#include <boost/preprocessor/cat.hpp>

template<bool condition> struct static_warn {};

template<> struct [[deprecated]] static_warn<false> {};

#define static_warn(cond) [[maybe_unused]] static static_warn<cond> BOOST_PP_CAT(static_warning_L, __LINE__)
