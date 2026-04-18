#include "raylib.h"
#include <emscripten/emscripten.h>
#include <stdio.h>

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

  Texture2D texture = LoadTexture("assets/sprites/background-day.png");

  MainLoop(draw, 60, 1);

  return 0;
}
