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
      active_tile_textures_{
          {PlayerKind::p1, IMG_Load("assets/img/tile_r.png")},
          {PlayerKind::p2, IMG_Load("assets/img/tile_y.png")},
          {PlayerKind::p3, IMG_Load("assets/img/tile_b.png")},
          {PlayerKind::p4, IMG_Load("assets/img/tile_g.png")}},
      tile_textures_{
          {PlayerKind::p1, IMG_Load("assets/img/tile_r_inactive.png")},
          {PlayerKind::p2, IMG_Load("assets/img/tile_y_inactive.png")},
          {PlayerKind::p3, IMG_Load("assets/img/tile_b_inactive.png")},
          {PlayerKind::p4, IMG_Load("assets/img/tile_g_inactive.png")}},
      activationKeyMap_{{
          {miyoo::BTN_A, PlayerKind::p1},
          {miyoo::BTN_B, PlayerKind::p2},
          {miyoo::BTN_X, PlayerKind::p3},
          {miyoo::BTN_Y, PlayerKind::p4},
      }},
      directionMap_{{
          {miyoo::BTN_UP, Direction::kUp},
          {miyoo::BTN_DOWN, Direction::kDown},
          {miyoo::BTN_LEFT, Direction::kLeft},
          {miyoo::BTN_RIGHT, Direction::kRight},
      }},
      world_{}, timer_{kGameStepTime}, ts_{SDL_GetTicks()},
      currentPlayer_{PlayerKind::p1} {}

void GameScene::Activate() {
  world_.points = 0;
  world_.snakes.clear();
  world_.foods.clear();
  timer_.Reset();
  tasks_.clear();

  tasks_.push_back({[this]() { return world_.points >= 5; },
                    [this]() {
                      world_.snakes.at(PlayerKind::p2).activate();
                      spawnFood(PlayerKind::p2);
                    }});

  tasks_.push_back({[this]() { return world_.points >= 10; },
                    [this]() {
                      world_.snakes.at(PlayerKind::p3).activate();
                      spawnFood(PlayerKind::p3);
                    }});

  tasks_.push_back({[this]() { return world_.points >= 15; },
                    [this]() {
                      world_.snakes.at(PlayerKind::p4).activate();
                      spawnFood(PlayerKind::p4);
                    }});

  spawnSnakeAt(PlayerKind::p1, {0, 0}, Direction::kRight, 5);
  spawnSnakeAt(PlayerKind::p2, {kGameFieldWidth - 1, 0}, Direction::kDown, 5);
  spawnSnakeAt(PlayerKind::p3, {0, kGameFieldHeight - 1}, Direction::kUp, 5);
  spawnSnakeAt(PlayerKind::p4, {kGameFieldWidth - 1, kGameFieldHeight - 1},
               Direction::kLeft, 5);

  spawnFood(PlayerKind::p1);
  world_.snakes.at(PlayerKind::p1).activate();
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
      auto newSnake = snakes.find(activationKey.second);
      if (newSnake != snakes.end() && newSnake->second.isPlayable()) {
        currentPlayer_ = activationKey.second;
        break;
      }
    }
  }

  auto &foods = world_.foods;

  auto currentSnake = snakes.find(currentPlayer_);

  if (currentSnake != snakes.end()) {
    for (auto &&directionKey : directionMap_) {
      if (keyState[directionKey.first]) {
        if (currentSnake->second.getDirection() == directionKey.second) {
          // Speed up snake movement 2x
          timer_.Update(dt);
        } else {
          currentSnake->second.setDirection(directionKey.second);
        }
      }
    }
  } else {
    Activate();
  }

  if (timer_.IsPassed()) {
    timer_.Reset();

    // move all snakes and erase all that collided
    for (auto it = snakes.begin(); it != snakes.end();) {
      auto &snake = it->second;

      if (snake.isPlayable()) {
        auto moveResult = snake.move();
        if (moveResult == MoveResult::kCollision) {
          it = snakes.erase(it);
          continue;
        }

        if (moveResult == MoveResult::kFoodTaken) {
          world_.points++;
          auto coords = snake.tiles[0];
          foods.erase(std::remove_if(foods.begin(), foods.end(),
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
      }

      ++it;
    }

    for (size_t y = 0; y < kGameFieldHeight; y++) {
      for (size_t x = 0; x < kGameFieldWidth; x++) {
        world_.field[x][y].state = TileState::kEmpty;
      }
    }

    for (auto &&snake : snakes) {
      for (auto &&tile : snake.second.tiles) {
        world_.field[tile.x][tile.y].state = TileState::kPlayer;
        world_.field[tile.x][tile.y].occupiedBy = snake.second.playerKind;
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
  SDL_Rect tileRect{};
  tileRect.w = kTileSize;
  tileRect.h = kTileSize;

  for (size_t y = 0; y < kGameFieldHeight; y++) {
    for (size_t x = 0; x < kGameFieldWidth; x++) {
      const auto &tile = world_.field[x][y];
      if (tile.state != TileState::kEmpty) {
        tileRect.x = x * kTileSize;
        tileRect.y = y * kTileSize;

        auto texture = tile_textures_[tile.occupiedBy];
        if (currentPlayer_ == tile.occupiedBy) {
          texture = active_tile_textures_[tile.occupiedBy];
        }

        SDL_BlitSurface(texture, nullptr, screen_, &tileRect);
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

bool GameScene::spawnSnakeAt(PlayerKind kind, Tile coords, Direction direction,
                             size_t length) {
  auto snake = Snake{kGameFieldWidth, kGameFieldHeight, kind, &world_};

  if (!snake.spawn(coords.x, coords.y, direction, length)) {
    return false;
  }

  world_.snakes.insert({kind, snake});

  return true;
}

bool GameScene::spawnSnake(PlayerKind kind) {
  auto coords = getRandomFreeTile();
  if (coords == kInvalidTile) {
    return false;
  }

  return spawnSnakeAt(kind, coords, Direction::kRight, 5);
}
