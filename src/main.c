#include "raylib.h"
#include <emscripten/emscripten.h>
#include <stdio.h>

#define MainLoop emscripten_set_main_loop

static const int screenWidth = 288;
static const int screenHeight = 512;

Texture2D background_texture;
Texture2D message_texture;

int scroll = 0;

void draw_message() {
  if (!IsTextureValid(message_texture)) {
    message_texture = LoadTexture("assets/sprites/message.png");
  }

  float centerX = (screenWidth - message_texture.width) / 2.0f;
  float centerY = (screenHeight - message_texture.height) / 2.0f;

  DrawTexture(message_texture, centerX, centerY, WHITE);
}

void draw_background() {
  if (!IsTextureValid(background_texture)) {
    background_texture = LoadTexture("assets/sprites/background-day.png");
  }

  static const int scrollSpeed = 1;

  scroll -= scrollSpeed;

  DrawTexture(background_texture, scroll % screenWidth, 0, WHITE);
  DrawTexture(background_texture, (scroll % screenWidth) + screenWidth, 0, WHITE);
}

void draw(void) {
  BeginDrawing();

  draw_background();

  draw_message();

  EndDrawing();
}

int main() {
  static const char *title = "rayflap";

  InitWindow(screenWidth, screenHeight, title);

  MainLoop(draw, 60, 1);

  return 0;
}
