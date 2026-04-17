#include "raylib.h"
#include <stdio.h>
#include <emscripten/emscripten.h>

void draw(void) {
  BeginDrawing();

  ClearBackground(RAYWHITE);

  DrawText(TextFormat("Hello World"), 190, 200, 20, DARKGRAY);

  EndDrawing();
}

int main() {
  static const char *title = "rayflap";
  static const int screenWidth = 480;
  static const int screenHeight = 720;

  InitWindow(screenWidth, screenHeight, title);

  emscripten_set_main_loop(draw, 60, 1);

  return 0;
}
