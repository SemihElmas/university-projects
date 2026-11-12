# Snake – SDL2 Game (C++)

A classic Snake game with 2D graphics, written in C++ with the SDL2 library.
Programming course project, Gdańsk University of Technology.

## Features
- Arrow-key controls on a 20-pixel grid
- Snake grows each time it eats food; food never spawns on the snake
- Game speeds up by 20% every 10 seconds
- **Auto-turn:** when the snake reaches a wall it turns right (or left, if right is blocked) instead of crashing
- Game over on self-collision – press **N** to restart or **Esc** to quit
- Elapsed-time display and text rendered from a bitmap font (`cs8x8.bmp`)

## How to run
Requires SDL2 (e.g. via Visual Studio with the SDL2 development libraries).
Keep `cs8x8.bmp` in the same folder as the executable.

## Credits
The bitmap font (`cs8x8.bmp`) comes from the course's SDL project template.
