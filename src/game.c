#include "raylib.h"
#include <emscripten/emscripten.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define MainLoop emscripten_set_main_loop
#define SeedRandom srand(time(NULL))

/* Properties */
static const char *title = "rayflap";
static const int screenWidth = 288;
static const int screenHeight = 512;

/* Textures */
Texture2D backgroundTexture;
Texture2D messageTexture;
Texture2D birdTexture;
Texture2D baseTexture;

/* Game State */
typedef enum { TITLE, PLAY } Screen;
Screen screen = TITLE;
float backgroundScroll = 0.0f;
float baseScroll = 0.0f;
char *birdColor = NULL;

void draw_message() {
  if (screen != TITLE)
    return;

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

  static const float scrollSpeed = 0.5f;

  backgroundScroll -= scrollSpeed;

  DrawTexture(backgroundTexture,
              (int)fmodf(backgroundScroll, (float)screenWidth), 0, WHITE);

  DrawTexture(backgroundTexture,
              (int)fmodf(backgroundScroll, (float)screenWidth) + screenWidth, 0,
              WHITE);
}

void draw_bird() {
  if (birdColor == NULL) {
    char *birdColors[3] = {"red", "blue", "yellow"};

    birdColor =
        birdColors[rand() % (sizeof(birdColors) / sizeof(birdColors[0]))];
  }

  if (!IsTextureValid(birdTexture)) {
    char *texturePath;

    asprintf(&texturePath, "assets/sprites/%sbird-downflap.png", birdColor);

    birdTexture = LoadTexture(texturePath);
  }

  float centerX = (screenWidth - birdTexture.width) / 2.0f;

  float centerY = (screenHeight - birdTexture.height) / 2.0f;

  float messageOffsetY = 0;

  if (IsTextureValid(messageTexture)) {
    messageOffsetY = (messageTexture.height - birdTexture.height) / 5.1f;
  }

  DrawTexture(birdTexture, centerX, centerY + messageOffsetY, WHITE);
}

void draw_base() {
  if (!IsTextureValid(baseTexture)) {
    baseTexture = LoadTexture("assets/sprites/base.png");
  }

  float bottomY =
      screenHeight - baseTexture.height + (baseTexture.height / 2.0f);

  static const float scrollSpeed = 1.2f;

  baseScroll -= scrollSpeed;

  DrawTexture(baseTexture, (int)fmodf(baseScroll, (float)screenWidth), bottomY,
              WHITE);

  DrawTexture(baseTexture,
              (int)fmodf(baseScroll, (float)screenWidth) + screenWidth, bottomY,
              WHITE);
}

void draw() {
  BeginDrawing();

  draw_background();

  draw_base();

  draw_message();

  draw_bird();

  EndDrawing();
}

void input() {
  if (!IsMouseButtonReleased(0))
    return;

  if (screen == TITLE) {
    screen = PLAY;
  }
}

void update(void) {
  input();

  draw();
}

int main() {
  SeedRandom;

  InitWindow(screenWidth, screenHeight, title);

  MainLoop(update, 60, 1);

  return 0;
}
