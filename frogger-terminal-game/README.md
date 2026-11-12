# Frogger – Terminal Game (C++)

A Frogger-style game played in the terminal: guide the frog across a road full of cars to reach the goal before the timer runs out.
Programming course project, Gdańsk University of Technology.

## Features
- Arrow-key controls, rendered in the terminal with the curses library (PDCurses on Windows)
- Cars with random direction and speed, which change speed while driving
- **Friendly cars** (blue) that transport the frog to a random spot instead of hitting it
- Static obstacles, lives, a countdown timer and a points system
- All game settings (map size, number of cars, speeds, lives, timer, colours) loaded from `config.txt`
- Fixed-tick game loop (100 ms)

## How to run
Requires a curses library – PDCurses on Windows (Visual Studio), ncurses on Linux/macOS.
Keep `config.txt` in the same folder as the executable.
