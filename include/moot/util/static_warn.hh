#pragma once

#include <boost/preprocessor/cat.hpp>

template<bool condition> struct static_warning {};

template<> struct [[deprecated]] static_warning<false> {};

#define static_warn(cond) [[maybe_unused]] static static_warning<cond> BOOST_PP_CAT(static_warning_L, __LINE__)
