cmake_minimum_required(VERSION 3.30)

add_library(moot STATIC)

target_include_directories(moot PRIVATE include)
target_include_directories(moot SYSTEM PUBLIC lib)
target_include_directories(moot SYSTEM INTERFACE include)

target_compile_features(moot PUBLIC cxx_std_26)

target_compile_options(moot PRIVATE
	-Weverything
	-Wno-c++98-compat
	-Wno-c99-designator
	-Wno-unsafe-buffer-usage
	-Wno-pre-c++14-compat
	-Wno-switch-enum
	-Wno-padded
	-Wno-exit-time-destructors
	-Wno-global-constructors
	-Wno-c++20-compat
	-Wno-c++20-extensions
	-Wno-pre-c++20-compat-pedantic
	-Wno-implicit-int-float-conversion
	-Wno-missing-field-initializers
)

file(GLOB_RECURSE source_files src/*.cc)
target_sources(moot PRIVATE ${source_files})

target_link_libraries(moot PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	lua
)
