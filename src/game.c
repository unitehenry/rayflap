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
Texture2D pipeTexture;
Texture2D scoreTexture[10];

/* Game State */
typedef enum { TITLE, PLAY } Screen;
Screen screen = TITLE;
bool isPaused = false;
float backgroundScroll = 0.0f;
float baseScroll = 0.0f;
char *birdColor = NULL;
float birdRotate = 0.0f;
float thrust = 0.0f;
float gravity = 0.0f;

/* Entities */
Rectangle birdRect = {0, 0, 0.0f, 0.0f};
Rectangle baseRect1 = {0, 0, 0.0f, 0.0f};
Rectangle baseRect2 = {0, 0, 0.0f, 0.0f};
Rectangle pipeRects[4] = {
    {0, 0, 0.0f, 0.0f},
    {0, 0, 0.0f, 0.0f},
    {0, 0, 0.0f, 0.0f},
    {0, 0, 0.0f, 0.0f},
};

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

void draw_pipes() {
  if (!IsTextureValid(pipeTexture)) {
    pipeTexture = LoadTexture("assets/sprites/pipe-green.png");
  }

  if (screen == TITLE)
    return;

  for (int i = 0; i < sizeof(pipeRects) / sizeof(pipeRects[0]); i++) {
    bool topPipe = i % 2 != 0;

    Rectangle source = {0, 0, (float)pipeTexture.width,
                        topPipe ? -(float)pipeTexture.height
                                : (float)pipeTexture.height};

    Vector2 origin = {0, 0};

    float rotate = 0.0f;

    DrawTexturePro(pipeTexture, source, pipeRects[i], origin, rotate, WHITE);
  }
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

  if (screen == TITLE) {
    birdTexture = downFlapTexture;
  }

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

  Rectangle source = {0, 0, (float)baseTexture.width,
                      (float)baseTexture.height};

  Vector2 origin = {0, 0};

  baseRect1.x = (int)fmodf(baseScroll, (float)screenWidth);
  baseRect1.y = bottomY;
  baseRect1.width = (float)baseTexture.width;
  baseRect1.height = (float)baseTexture.height;

  baseRect2.x = (int)fmodf(baseScroll, (float)screenWidth) + screenWidth;
  baseRect2.y = bottomY;
  baseRect2.width = (float)baseTexture.width;
  baseRect2.height = (float)baseTexture.height;

  DrawTexturePro(baseTexture, source, baseRect1, origin, 0.0f, WHITE);

  DrawTexturePro(baseTexture, source, baseRect2, origin, 0.0f, WHITE);
}

void draw_score() {
  if (!IsTextureValid(scoreTexture[0])) {
    for (int i = 0; i < sizeof(scoreTexture) / sizeof(scoreTexture[0]); i++) {
      char *texturePath;

      asprintf(&texturePath, "assets/sprites/%i.png", i);

      scoreTexture[i] = LoadTexture(texturePath);
    }
  }

  if (screen == TITLE) {
    return;
  }

  int testScore = 123;

  char *renderScore;

  asprintf(&renderScore, "%i", testScore);

  float scoreY = 0.0f + (scoreTexture[0].height * 1.5f);

  for(int i = 0; i < sizeof(renderScore) - 1; i++) {
    float scoreX = ((screenWidth - scoreTexture[0].width) / 2.0f) + (scoreTexture[0].width * i);

    DrawTexture(scoreTexture[renderScore[i] - '0'], scoreX, scoreY, WHITE);
  }
}

void draw() {
  BeginDrawing();

  if (isPaused) {
    EndDrawing();
    return;
  }

  draw_background();

  draw_pipes();

  draw_base();

  draw_message();

  draw_bird();

  draw_score();

  EndDrawing();
}

Vector2 random_pipe_y() {
  if (!IsTextureValid(pipeTexture)) {
    Vector2 origin = {0.0f, 0.0f};
    return origin;
  }

  float unit = screenHeight / 20.0f;

  float lowerBound = pipeTexture.height - (unit * 5);

  float upperBound = screenHeight - pipeTexture.height + (unit * 8);

  float pipeY = lowerBound + fmodf(rand(), (upperBound - lowerBound + 1));

  float gap = (unit * 4);

  float bottomPipeY = pipeY;

  float topPipeY = pipeY - gap - pipeTexture.height;

  Vector2 pipeVector = {bottomPipeY, topPipeY};

  return pipeVector;
}

void reset_pipes() {
  if (!IsTextureValid(pipeTexture))
    return;

  for (int i = 0; i < sizeof(pipeRects) / sizeof(pipeRects[0]); i += 2) {
    float startX = i > 0 ? pipeRects[i - 1].x + screenWidth - pipeTexture.width
                         : (screenWidth + pipeTexture.width);

    Vector2 pipeY = random_pipe_y();

    Rectangle bottomPipeRect = pipeRects[i];

    bottomPipeRect.x = startX;
    bottomPipeRect.y = pipeY.x;
    bottomPipeRect.width = (float)pipeTexture.width;
    bottomPipeRect.height = (float)pipeTexture.height;

    pipeRects[i] = bottomPipeRect;

    Rectangle topPipeRect = pipeRects[i + 1];

    topPipeRect.x = startX;
    topPipeRect.y = pipeY.y;
    topPipeRect.width = (float)pipeTexture.width;
    topPipeRect.height = (float)pipeTexture.height;

    pipeRects[i + 1] = topPipeRect;
  }
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

void collide() {
  Rectangle baseRects[2] = {baseRect1, baseRect2};

  for (int i = 0; i < (sizeof(baseRects) / sizeof(baseRects[0])); i++) {
    Rectangle baseRect = baseRects[i];

    if (CheckCollisionRecs(birdRect, baseRect)) {
      isPaused = true;
    }
  }

  for (int i = 0; i < sizeof(pipeRects) / sizeof(pipeRects[0]); i++) {
    Rectangle pipeRect = pipeRects[i];

    if (CheckCollisionRecs(birdRect, pipeRect)) {
      isPaused = true;
    }
  }
}

void reset() {
  reset_pipes();
  reset_bird();
  isPaused = false;
  screen = TITLE;
  birdColor = NULL;
}

void input() {
  if (!IsMouseButtonReleased(0))
    return;

  if (isPaused) {
    reset();
    return;
  }

  if (screen == TITLE) {
    screen = PLAY;
  }

  thrust = 3.75f;
}

void update() {
  input();

  if (screen == TITLE) {
    reset_pipes();
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

  static const float scrollSpeed = 1.2f;

  float offscreenX = 0.0f - pipeTexture.width;

  int lastIdx = (sizeof(pipeRects) / sizeof(pipeRects[0])) - 1;

  for (int i = 0; i < sizeof(pipeRects) / sizeof(pipeRects[0]); i += 2) {
    float startX = (i > 0 ? pipeRects[i - 1].x : pipeRects[lastIdx].x) +
                   screenWidth - pipeTexture.width;

    Rectangle bottomPipeRect = pipeRects[i];

    bottomPipeRect.x -= scrollSpeed;
    bottomPipeRect.width = (float)pipeTexture.width;
    bottomPipeRect.height = (float)pipeTexture.height;

    Rectangle topPipeRect = pipeRects[i + 1];

    topPipeRect.x -= scrollSpeed;
    topPipeRect.width = (float)pipeTexture.width;
    topPipeRect.height = (float)pipeTexture.height;

    if (topPipeRect.x < offscreenX) {
      Vector2 pipeY = random_pipe_y();

      bottomPipeRect.x = startX;
      bottomPipeRect.y = pipeY.x;

      topPipeRect.x = startX;
      topPipeRect.y = pipeY.y;
    }

    pipeRects[i] = bottomPipeRect;
    pipeRects[i + 1] = topPipeRect;
  }

  collide();
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
