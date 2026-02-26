# Vintage House Explorer (raylib + C++)

A small first-person 3D exploration game made with **raylib** and C++.
You can move through a simple vintage-style house with multiple rooms.

## Controls

- `W A S D`: move
- Mouse: look around
- `Esc`: quit

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/vintage_house
```

## Notes

- The game uses a first-person camera and simple collision to keep the player inside the house.
- Rooms include Library, Dining Room, Parlor, Bedroom, and a central hallway.
