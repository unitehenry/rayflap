#include "raylib.h"
#include <stdio.h>
#include <emscripten/emscripten.h>

#define MainLoop emscripten_set_main_loop

void draw(void) {
  BeginDrawing();

  ClearBackground(RAYWHITE);

  DrawText(TextFormat("Hello World"), 190, 200, 20, DARKGRAY);

  EndDrawing();
}

int main() {
  static const char *title = "rayflap";
  static const int screenWidth = 640;
  static const int screenHeight = 960;

  InitWindow(screenWidth, screenHeight, title);

  MainLoop(draw, 60, 1);

  return 0;
}
