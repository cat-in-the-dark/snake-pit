#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "game_scene.h"

#include <SDL/SDL_events.h>

#include <SDL/SDL_image.h>

#include "miyoo.h"

constexpr auto kGameStepTime = 200.0f;

GameScene::GameScene(SceneManager *sm, SDL_Surface *screen)
    : sm_(sm), screen_(screen),
      tile_textures_{{PlayerKind::p1, IMG_Load("assets/img/tile_r.png")},
                     {PlayerKind::p2, IMG_Load("assets/img/tile_y.png")},
                     {PlayerKind::p3, IMG_Load("assets/img/tile_b.png")},
                     {PlayerKind::p4, IMG_Load("assets/img/tile_g.png")}},
      activationKeyMap_{{
          {miyoo::BTN_A, PlayerKind::p1},
          {miyoo::BTN_B, PlayerKind::p2},
          {miyoo::BTN_X, PlayerKind::p3},
          {miyoo::BTN_Y, PlayerKind::p4},
      }},
      world_{}, timer_{kGameStepTime}, ts_{SDL_GetTicks()},
      currentPlayer_{PlayerKind::p1} {}

void GameScene::Activate() {
  world_.snakes.clear();
  world_.foods.clear();
  timer_.Reset();
  tasks_.clear();

  tasks_.push_back({[this]() { return world_.points >= 5; },
                    [this]() {
                      spawnSnake(PlayerKind::p2);
                      spawnFood(PlayerKind::p2);
                    }});

  tasks_.push_back({[this]() { return world_.points >= 10; },
                    [this]() {
                      spawnSnake(PlayerKind::p3);
                      spawnFood(PlayerKind::p3);
                    }});

  tasks_.push_back({[this]() { return world_.points >= 20; },
                    [this]() {
                      spawnSnake(PlayerKind::p4);
                      spawnFood(PlayerKind::p4);
                    }});

  spawnSnake(PlayerKind::p1);
  spawnFood(PlayerKind::p1);
}

void GameScene::Exit() {}

void GameScene::Update() {
  auto ts1 = SDL_GetTicks();
  auto dt = static_cast<float>(ts1 - ts_);
  timer_.Update(dt);
  for (auto &&task : tasks_) {
    task.Update();
  }
  ts_ = ts1;

  auto *keyState = SDL_GetKeyState(nullptr);

  auto &snakes = world_.snakes;

  for (auto &&activationKey : activationKeyMap_) {
    if (keyState[activationKey.first]) {
      auto newSnake = std::find_if(
          snakes.begin(), snakes.end(), [&activationKey](const Snake &snake) {
            return snake.playerKind == activationKey.second;
          });
      if (newSnake != snakes.end()) {
        currentPlayer_ = activationKey.second;
        break;
      }
    }
  }

  auto &foods = world_.foods;

  auto currentSnake =
      std::find_if(snakes.begin(), snakes.end(), [this](const Snake &snake) {
        return snake.playerKind == currentPlayer_;
      });

  if (currentSnake != snakes.end()) {
    if (keyState[miyoo::BTN_UP]) {
      currentSnake->setDirection(Direction::kUp);
    } else if (keyState[miyoo::BTN_DOWN]) {
      currentSnake->setDirection(Direction::kDown);
    } else if (keyState[miyoo::BTN_LEFT]) {
      currentSnake->setDirection(Direction::kLeft);
    } else if (keyState[miyoo::BTN_RIGHT]) {
      currentSnake->setDirection(Direction::kRight);
    }
  }

  if (timer_.IsPassed()) {
    timer_.Reset();

    snakes.erase(
        std::remove_if(snakes.begin(), snakes.end(),
                       [this, &foods](Snake &snake) {
                         auto moveResult = snake.move();
                         if (moveResult == MoveResult::kCollision) {
                           return true;
                         }

                         if (moveResult == MoveResult::kFoodTaken) {
                           world_.points++;
                           auto coords = snake.tiles[0];
                           foods.erase(
                               std::remove_if(foods.begin(), foods.end(),
                                              [&coords](const Food &food) {
                                                if (food.coords == coords) {
                                                  return true;
                                                }

                                                return false;
                                              }),
                               foods.end());
                           spawnFood(snake.playerKind);
                         } else {
                           snake.moveTail();
                         }

                         return false;
                       }),
        snakes.end());

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

    for (auto &&food : foods) {
      const auto &tile = food.coords;
      world_.field[tile.x][tile.y].state = TileState::kFood;
      world_.field[tile.x][tile.y].occupiedBy = food.kind;
    }
  }
}

void GameScene::Draw() {
  SDL_Rect textCentered{};
  textCentered.w = kTileSize;
  textCentered.h = kTileSize;

  for (size_t y = 0; y < kGameFieldHeight; y++) {
    for (size_t x = 0; x < kGameFieldWidth; x++) {
      const auto &tile = world_.field[x][y];
      if (tile.state != TileState::kEmpty) {
        textCentered.x = x * kTileSize;
        textCentered.y = y * kTileSize;

        SDL_BlitSurface(tile_textures_[tile.occupiedBy], nullptr, screen_,
                        &textCentered);
      }
    }
  }
}

Tile GameScene::getRandomFreeTile() const {
  constexpr auto invalidIndex = static_cast<size_t>(-1);
  constexpr auto triesCount = 10;
  size_t index = invalidIndex;
  size_t rndIdx = 0;
  for (int i = 0; i < triesCount; i++) {
    rndIdx = random() % kGameFieldSize;
    auto x = rndIdx / kGameFieldHeight;
    auto y = rndIdx % kGameFieldHeight;

    if (world_.field[x][y].state == TileState::kEmpty) {
      index = rndIdx;
      break;
    }
  }

  // FIXME: make this less stupid
  if (index == invalidIndex) {
    // this should have largest common divider of 1 with kGameFieldSize
    constexpr auto increment = 1151;
    for (int i = 0; i < kGameFieldSize; i++) {
      rndIdx = (rndIdx + increment) % kGameFieldSize;
      auto x = rndIdx / kGameFieldHeight;
      auto y = rndIdx % kGameFieldHeight;

      if (world_.field[x][y].state == TileState::kEmpty) {
        index = rndIdx;
        break;
      }
    }
  }

  if (index == invalidIndex) {
    return kInvalidTile;
  }

  uint32_t x = rndIdx / kGameFieldHeight;
  uint32_t y = rndIdx % kGameFieldHeight;
  return {x, y};
}

bool GameScene::spawnFood(PlayerKind kind) {
  auto tile = getRandomFreeTile();
  if (tile == kInvalidTile) {
    return false;
  }

  world_.foods.push_back({tile, kind});
  return true;
}

bool GameScene::spawnSnake(PlayerKind kind) {
  auto s1 = Snake{kGameFieldWidth, kGameFieldHeight, kind, &world_};

  auto coords = getRandomFreeTile();
  if (coords == kInvalidTile) {
    return false;
  }

  if (!s1.spawn(coords.x, coords.y, Direction::kRight, 5)) {
    return false;
  }

  world_.snakes.push_back(s1);
  return true;
}
