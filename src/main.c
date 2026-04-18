#include "raylib.h"
#include <emscripten/emscripten.h>
#include <stdio.h>

#define MainLoop emscripten_set_main_loop

Texture2D background_texture;
Texture2D message_texture;
int scrollX = 0;

void draw_message() {
  if (!IsTextureValid(message_texture)) {
    message_texture = LoadTexture("assets/sprites/message.png");
  }

  float centerX = (288 - message_texture.width) / 2.0f;
  float centerY = (512 - message_texture.height) / 2.0f;

  DrawTexture(message_texture, centerX, centerY, WHITE);
}

void draw_background() {
  if (!IsTextureValid(background_texture)) {
    background_texture = LoadTexture("assets/sprites/background-day.png");
  }

  static const int scrollSpeed = 1;

  scrollX -= scrollSpeed;

  DrawTexture(background_texture, scrollX % 288, 0, WHITE);
  DrawTexture(background_texture, (scrollX % 288) + 288, 0, WHITE);
}

void draw(void) {
  BeginDrawing();

  draw_background();

  draw_message();

  EndDrawing();
}

int main() {
  static const char *title = "rayflap";
  static const int screenWidth = 288;
  static const int screenHeight = 512;

  InitWindow(screenWidth, screenHeight, title);

  MainLoop(draw, 60, 1);

  return 0;
}
