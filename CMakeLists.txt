cmake_minimum_required(VERSION 3.30)

set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
add_subdirectory(lib/SFML SYSTEM)

add_library(moot STATIC)

target_include_directories(moot PRIVATE include)
target_include_directories(moot SYSTEM PUBLIC lib)
target_include_directories(moot SYSTEM INTERFACE include)

target_compile_features(moot PUBLIC cxx_std_26)

set(DISABLED_WARNINGS
	missing-field-initializers
)
list(TRANSFORM DISABLED_WARNINGS PREPEND "-Wno-")

set(DISABLED_CLANG_WARNINGS
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
)
list(TRANSFORM DISABLED_CLANG_WARNINGS PREPEND "-Wno-")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(moot PRIVATE
		-Weverything
		-Wlarge-by-value-copy
		${DISABLED_CLANG_WARNINGS}
	)
else()
	target_compile_options(moot PRIVATE
		-Wall
		-Wextra
	)
endif()

target_compile_options(moot PRIVATE
	-Winvalid-constexpr
	-Wframe-larger-than=4096
	${DISABLED_WARNINGS}
)

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
