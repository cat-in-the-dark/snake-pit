#include <algorithm>
#include <iostream>

#include "game_scene.h"

#include <SDL/SDL_events.h>

#include <SDL/SDL_image.h>

#include "miyoo.h"

constexpr auto kGameStepTime = 100.0f;

GameScene::GameScene(SceneManager *sm, SDL_Surface *screen)
    : sm_(sm), screen_(screen), tile_tex_(IMG_Load("assets/img/tile.png")),
      world_{}, timer_{kGameStepTime}, ts_{SDL_GetTicks()} {}

void GameScene::Activate() {
  snakes.clear();
  timer_.Reset();

  auto s1 = Snake{kGameFieldWidth, kGameFieldHeight, PlayerKind::p1, &world_};
  if (!s1.spawn(10, 20, Direction::kRight, 10)) {
    return;
  }

  snakes.push_back(s1);
}

void GameScene::Exit() {}

void GameScene::Update() {
  auto ts1 = SDL_GetTicks();
  auto dt = static_cast<float>(ts1 - ts_);
  timer_.Update(dt);
  ts_ = ts1;

  auto *keyState = SDL_GetKeyState(nullptr);
  if (keyState[miyoo::BTN_UP]) {
    snakes[0].setDirection(Direction::kUp);
  } else if (keyState[miyoo::BTN_DOWN]) {
    snakes[0].setDirection(Direction::kDown);
  } else if (keyState[miyoo::BTN_LEFT]) {
    snakes[0].setDirection(Direction::kLeft);
  } else if (keyState[miyoo::BTN_RIGHT]) {
    snakes[0].setDirection(Direction::kRight);
  }

  if (timer_.IsPassed()) {
    timer_.Reset();

    for (auto &&snake : snakes) {
      snake.moveTail();
    }

    snakes.erase(std::remove_if(snakes.begin(), snakes.end(),
                                [](Snake &snake) { return !snake.move(); }),
                 snakes.end());
  }

  for (size_t y = 0; y < kGameFieldHeight; y++) {
    for (size_t x = 0; x < kGameFieldWidth; x++) {
      world_.field[x][y].state = TileState::kEmpty;
    }
  }

  for (auto &&snake : snakes) {
    for (auto &&tile : snake.tiles) {
      world_.field[tile.x][tile.y].state = TileState::kPlayer;
      world_.field[tile.x][tile.y].occupiedBy = snake.playerKind;
    }
  }
}

void GameScene::Draw() {
  SDL_Rect textCentered{};
  textCentered.w = kTileSize;
  textCentered.h = kTileSize;

  for (size_t y = 0; y < kGameFieldHeight; y++) {
    for (size_t x = 0; x < kGameFieldWidth; x++) {
      if (world_.field[x][y].state == TileState::kPlayer) {
        textCentered.x = x * kTileSize;
        textCentered.y = y * kTileSize;

        SDL_BlitSurface(tile_tex_.get(), nullptr, screen_, &textCentered);
      }
    }
  }
}
