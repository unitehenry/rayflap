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
static const int maxGravity = 3;
static const int maxBirdRotate = 20.0f;

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
Rectangle birdRect = {0, 0, 0.0f, 0.0f};
float birdRotate = 0.0f;
float thrust = 0.0f;
float gravity = 0.0f;

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

  static Texture2D downFlapTexture;

  static Texture2D upFlapTexture;

  if (!IsTextureValid(birdTexture)) {
    char *texturePath;

    asprintf(&texturePath, "assets/sprites/%sbird-downflap.png", birdColor);

    downFlapTexture = LoadTexture(texturePath);

    SetTextureFilter(downFlapTexture, TEXTURE_FILTER_BILINEAR);

    asprintf(&texturePath, "assets/sprites/%sbird-upflap.png", birdColor);

    upFlapTexture = LoadTexture(texturePath);

    SetTextureFilter(upFlapTexture, TEXTURE_FILTER_BILINEAR);
  }

  birdTexture = thrust > 0 ? downFlapTexture : upFlapTexture;

  Rectangle source = {0, 0, (float)birdTexture.width,
                      (float)birdTexture.height};

  Vector2 origin = {0, 0};

  float rotate = 0.0f;

  if (screen == PLAY) {
    if (thrust > 0) {
      birdRotate += -4.0f;
      birdRotate = fmax(birdRotate, maxBirdRotate * -1);
    } else {
      birdRotate += 4.0f;
      birdRotate = fmin(birdRotate, maxBirdRotate);
    }

    rotate = birdRotate;
  }

  DrawTexturePro(birdTexture, source, birdRect, origin, rotate, WHITE);
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

void reset_bird() {
  if (!IsTextureValid(birdTexture))
    return;

  float centerX = (screenWidth - birdTexture.width) / 2.0f;

  float centerY = (screenHeight - birdTexture.height) / 2.0f;

  float messageOffsetY = 0;

  if (IsTextureValid(messageTexture)) {
    messageOffsetY = (messageTexture.height - birdTexture.height) / 5.2f;
  }

  birdRect.x = centerX;
  birdRect.y = centerY + messageOffsetY;
  birdRect.width = (float)birdTexture.width;
  birdRect.height = (float)birdTexture.height;
}

void input() {
  if (!IsMouseButtonReleased(0))
    return;

  if (screen == TITLE) {
    screen = PLAY;
  }

  thrust = 5.0f;
}

void update() {
  input();

  if (screen == TITLE) {
    reset_bird();
    return;
  }

  if (thrust > 0) {
    thrust -= .1;
    birdRect.y -= thrust;
    gravity = 1.0f;
  } else {
    if (gravity < maxGravity) {
      gravity += .1;
    }
    birdRect.y += gravity;
  }
}

void game_loop(void) {
  update();

  draw();
}

int main() {
  SeedRandom;

  InitWindow(screenWidth, screenHeight, title);

  MainLoop(game_loop, 60, 1);

  return 0;
}
