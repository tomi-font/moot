cmake_minimum_required(VERSION 3.30)

add_library(moot STATIC)

target_include_directories(moot PRIVATE include)
target_include_directories(moot SYSTEM PUBLIC lib)
target_include_directories(moot SYSTEM INTERFACE include)

target_compile_features(moot PUBLIC cxx_std_26)

# For clangd to not show warnings in header files.
set(PUBLIC_DISABLED_WARNINGS
	c++98-compat
	padded
	pre-c++14-compat
)
list(TRANSFORM PUBLIC_DISABLED_WARNINGS PREPEND "-Wno-")

target_compile_options(moot PUBLIC
	${PUBLIC_DISABLED_WARNINGS}
)

set(PRIVATE_DISABLED_WARNINGS
	c99-designator
	c++20-compat
	c++20-extensions
	exit-time-destructors
	global-constructors
	implicit-int-float-conversion
	missing-field-initializers
	packed-non-pod
	pre-c++20-compat-pedantic
	switch
	switch-default
	switch-enum
	unsafe-buffer-usage
)
list(TRANSFORM PRIVATE_DISABLED_WARNINGS PREPEND "-Wno-")

target_compile_options(moot PRIVATE
	-Weverything
	${PRIVATE_DISABLED_WARNINGS}
)

file(GLOB_RECURSE source_files src/*.cc)
target_sources(moot PRIVATE ${source_files})

target_link_libraries(moot PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	lua
)
