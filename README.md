# C Raylib Flappy Bird

A Flappy Bird clone made with raylib.

## Building this Project

### Prerequisites

Make sure you have the following dependencies installed before attempting to build.

- `git`
- `python`

### Build Steps

```bash
# 1. Install emsdk
source scripts/emsdk

# 2. Compile raylib
scripts/raylib

# 3. Build the game
scripts/build
```

## Running the Game

After running the following command, you can access the game at `http://localhost:8080/game.html`.

```bash
python -m http.server -d build 8080
```
