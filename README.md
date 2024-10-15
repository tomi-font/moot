## description

This:

- is a 2D video game engine built on top of the [SFML](https://sfml-dev.org).
- uses the [ECS](https://en.wikipedia.org/wiki/Entity_component_system) paradigm.
- is not made with external audience in mind.

## dependencies

#### toolchain

- Clang >= 17 (for C++ modules, `-Weverything` and `cxx_std_26` support)
- Ninja >= 1.11
- CMake >= 3.30

#### libraries

- libc++ 18
- Boost
- SFML 2.6.1
- Lua 5.4.6
