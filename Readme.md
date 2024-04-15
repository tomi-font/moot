## description

This:

- is a 2D video game engine written from scratch.
- uses the [ECS](https://en.wikipedia.org/wiki/Entity_component_system) paradigm.
- is not made with external audience in mind.

## state and direction

I am currently working towards making LLMs able to create games by themselves with the engine.
For that to happen, background work on making everything scriptable must be cleared first.

## dependencies

#### compiler

- Clang (for -Weverything)

#### libraries

- Boost.Mp11
- SFML 2.5.1 (graphics, window, system)
- Lua 5.4.6
- sol 3.3.0
