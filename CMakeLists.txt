cmake_minimum_required(VERSION 3.30)

add_library(moot STATIC)

target_include_directories(moot PUBLIC include)
target_include_directories(moot SYSTEM PUBLIC lib)

target_compile_features(moot PUBLIC cxx_std_26)

# TODO: make private
target_compile_options(moot PUBLIC
	-Weverything
	-Wno-c99-designator
	-Wno-c++98-compat
	-Wno-unsafe-buffer-usage
	-Wno-pre-c++14-compat
	-Wno-switch-enum
	-Wno-padded
	-Wno-shadow-field-in-constructor
	-Wno-exit-time-destructors
	-Wno-global-constructors
	-Wno-c++20-compat
	-Wno-c++20-extensions
	-Wno-pre-c++20-compat-pedantic
	-Wno-implicit-int-float-conversion
)

file(GLOB_RECURSE sources src/*)
target_sources(moot PRIVATE ${sources})

target_link_libraries(moot PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	lua
)
