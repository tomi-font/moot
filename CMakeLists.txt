cmake_minimum_required(VERSION 3.30)

add_library(moot STATIC)

target_include_directories(moot PUBLIC include)
target_include_directories(moot SYSTEM PUBLIC lib)

target_compile_features(moot PUBLIC cxx_std_26)
set_target_properties(moot PROPERTIES
	CXX_SCAN_FOR_MODULES ON
)

target_compile_options(moot PUBLIC
	-fmodules # Needed for ccache to know modules are enabled.
)

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
	-Wno-missing-designated-field-initializers
)

target_sources(moot PUBLIC
	FILE_SET sources_set
	TYPE CXX_MODULES
	FILES
	src/parsing/ComponentNames.cc
)

target_sources(moot PRIVATE
	src/Component/CCallback.cc
	src/Component/CHudRender.cc
	src/Component/CMove.cc
	src/Component/CRender.cc
	src/Component/CView.cc
	src/Component/CInput.cc
	src/Entity/Archetype.cc
	src/Entity/Template.cc
	src/Entity/TemplateStore.cc
	src/Entity/hash.cc
	src/Entity/Querier.cc
	src/Entity/Query.cc
	src/Event/Manager.cc
	src/Event/User.cc
	src/Property/Manager.cc
	src/Property/User.cc
	src/System/SInput.cc
	src/System/SPhysics.cc
	src/System/SRender.cc
	src/System/System.cc
	src/parsing/CallbackParameters.cc
	src/parsing/ComponentAttributes.cc
	src/parsing/Context.cc
	src/parsing/EntityFunctions.cc
	src/parsing/GlobalFunctions.cc
	src/parsing/TemplateAttributes.cc
	src/Game.cc
	src/Window.cc
	src/World.cc
)

target_link_libraries(moot PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	lua
)
