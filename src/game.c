#include "raylib.h"
#include <emscripten/emscripten.h>
#include <stdio.h>

#define MainLoop emscripten_set_main_loop

/* Properties */
static const char *title = "rayflap";
static const int screenWidth = 288;
static const int screenHeight = 512;

/* Textures */
Texture2D backgroundTexture;
Texture2D messageTexture;
Texture2D birdTexture;

/* Game State */
int scroll = 0;

void draw_message() {
  if (!IsTextureValid(messageTexture)) {
    messageTexture = LoadTexture("assets/sprites/message.png");
  }

  float centerX = (screenWidth - messageTexture.width) / 2.0f;

  float centerY = (screenHeight - messageTexture.height) / 2.0f;

  DrawTexture(messageTexture, centerX, centerY, WHITE);
}

void draw_background() {
  if (!IsTextureValid(backgroundTexture)) {
    backgroundTexture = LoadTexture("assets/sprites/background-day.png");
  }

  static const int scrollSpeed = 1;

  scroll -= scrollSpeed;

  DrawTexture(backgroundTexture, scroll % screenWidth, 0, WHITE);
  DrawTexture(backgroundTexture, (scroll % screenWidth) + screenWidth, 0,
              WHITE);
}

void draw_bird() {
  if (!IsTextureValid(birdTexture)) {
    birdTexture = LoadTexture("assets/sprites/redbird-downflap.png");
  }

  float centerX = (screenWidth - birdTexture.width) / 2.0f;

  float centerY = (screenHeight - birdTexture.height) / 2.0f;

  float messageOffsetY = 0;

  if (IsTextureValid(messageTexture)) {
    messageOffsetY = (messageTexture.height - birdTexture.height) / 5.1f;
  }

  DrawTexture(birdTexture, centerX, centerY + messageOffsetY, WHITE);
}

void draw(void) {
  BeginDrawing();

  draw_background();

  draw_message();

  draw_bird();

  EndDrawing();
}

int main() {
  InitWindow(screenWidth, screenHeight, title);

  MainLoop(draw, 60, 1);

  return 0;
}
