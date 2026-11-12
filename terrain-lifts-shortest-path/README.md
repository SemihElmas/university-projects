# Shortest Path over Terrain with Scheduled Lifts (C++)

Finds the fastest route across a grid-based height map, where the travelling time depends on the terrain and on one-way lifts that only depart at fixed intervals.

## Problem
- The map is a grid; every field has a height.
- You can move to the 4 neighbouring fields. Climbing from height B to a higher field A takes `A − B + 1` minutes; moving to a field of equal or lower height takes 1 minute.
- **Lifts** connect two fields one-way. Each lift departs only at multiples of its interval, so you may need to wait, and the ride itself takes a fixed number of minutes.
- Output: the minimum time from the start field to the target field.

## Solution
- **Dijkstra's algorithm** on the grid graph, where lifts are extra edges whose cost depends on the arrival time (waiting time + ride time). Dijkstra still works because arriving earlier never makes you arrive later.
- **Custom binary min-heap** (no STL) with dynamic resizing and lazy deletion of outdated entries.
- **Forward-star adjacency list** for lifts: compact arrays instead of per-node containers.
- Stops as soon as the target field is taken from the heap.

Tested on a 1000 × 1000 map with 2,000 lifts: about 0.1 s.

## Input format
1. Map width and height
2. Start position (column, row)
3. Target position (column, row)
4. Number of lifts, then for each lift: start (col, row), destination (col, row), ride time, departure interval
5. Heights of all fields, row by row

## How to run
```bash
g++ -O2 -o paths main.cpp
./paths < examplestxt    # prints 22
```
