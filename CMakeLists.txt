cmake_minimum_required(VERSION 3.30)

set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
add_subdirectory(lib/SFML SYSTEM)

add_library(moot STATIC)

target_include_directories(moot PUBLIC include)
target_include_directories(moot SYSTEM PUBLIC lib)

target_compile_features(moot PUBLIC cxx_std_26)

set(disabled_warnings
	missing-field-initializers
)
list(TRANSFORM disabled_warnings PREPEND "-Wno-")

set(disabled_clang_warnings
	c++98-compat
	c++98-compat-pedantic
	gnu-anonymous-struct
	inaccessible-base
	nested-anon-types
	padded
	pre-c++14-compat
	c99-designator
	c++20-compat
	c++20-extensions
	exit-time-destructors
	global-constructors
	implicit-int-float-conversion
	packed-non-pod
	pre-c++20-compat-pedantic
	switch
	switch-default
	switch-enum
	unsafe-buffer-usage
	shadow-field-in-constructor
	weak-vtables
	float-equal
	sign-conversion
)
list(TRANSFORM disabled_clang_warnings PREPEND "-Wno-")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	list(APPEND warnings
		-Weverything
		-Wlarge-by-value-copy
		${disabled_clang_warnings}
	)
else()
	list(APPEND warnings
		-Wall
		-Wextra
	)
endif()

list(APPEND warnings
	-Winvalid-constexpr
	-Wframe-larger-than=4096
	${disabled_warnings}
)

target_compile_options(moot PUBLIC ${warnings})

file(GLOB_RECURSE source_files src/*.cc)
target_sources(moot PRIVATE ${source_files})

target_link_libraries(moot
	PUBLIC
		sfml-graphics
		sfml-window
		sfml-system
	PRIVATE
		lua
)
