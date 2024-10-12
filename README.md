## description

This:

- is a 2D video game engine built on top of the [SFML](https://sfml-dev.org).
- uses the [ECS](https://en.wikipedia.org/wiki/Entity_component_system) paradigm.
- is not made with external audience in mind.

## dependencies

#### build tools

- CMake >= 3.30
- Clang >= 17 (for `-Weverything` and `cxx_std_26` support)

#### libraries

- Boost.Mp11
- SFML 2.6.1
- Lua 5.4.6
- sol 3.3.0