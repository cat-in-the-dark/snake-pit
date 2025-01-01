#include "snake.h"

#include <iostream>

#include <map>
#include <vector>

#include "game_world.h"

static std::map<Direction, Direction> incompatibleDirections = {
    {Direction::kUp, Direction::kDown},
    {Direction::kDown, Direction::kUp},
    {Direction::kLeft, Direction::kRight},
    {Direction::kRight, Direction::kLeft}};

Snake::Snake(uint32_t fieldWidth, uint32_t fieldHeight, PlayerKind playerKind,
             GameWorld *world)
    : fieldWidth_(fieldWidth), fieldHeight_(fieldHeight),
      playerKind(playerKind), tiles{},
      world_(world), canSetDirection_{true}, isPlayable_{false} {}

bool Snake::spawn(uint32_t startX, uint32_t startY, Direction startDirection,
                  size_t initLength) {
  if (startX >= fieldWidth_ || startY >= fieldHeight_) {
    return false;
  }

  currentDir_ = startDirection;

  tiles.clear();
  auto tile = Tile{startX, startY};
  tiles.push_front(tile);
  for (size_t i = 0; i < initLength; i++) {
    tile = nextTile(tile, startDirection);
    if (!isValidTile(tile)) {
      return false;
    }

    tiles.push_front(tile);
  }

  return true;
}

void Snake::moveTail() {
  if (!tiles.empty()) {
    tiles.pop_back();
  }
}

void Snake::setDirection(Direction dir) {
  if (!canSetDirection_) {
    return;
  }

  if (incompatibleDirections[currentDir_] == dir) {
    return;
  }

  currentDir_ = dir;
  canSetDirection_ = false;
}

Direction Snake::getDirection() const { return currentDir_; }

void Snake::activate() { isPlayable_ = true; }

bool Snake::isPlayable() const { return isPlayable_; }

MoveResult Snake::move() {
  canSetDirection_ = true;

  auto next = nextTile(tiles.front(), currentDir_);
  if (!isValidTile(next)) {
    return MoveResult::kCollision;
  }

  if (!isEmptyTile(next)) {
    if (isFoodTile(next)) {
      tiles.push_front(next);
      return MoveResult::kFoodTaken;
    } else {
      return MoveResult::kCollision;
    }
  }

  tiles.push_front(next);
  return MoveResult::kSuccess;
}

Tile Snake::nextTile(const Tile &tile, Direction dir) {
  switch (dir) {
  case Direction::kUp: {
    return {tile.x, tile.y - 1};
  }

  case Direction::kDown: {
    return {tile.x, tile.y + 1};
  }

  case Direction::kLeft: {
    return {tile.x - 1, tile.y};
  }

  case Direction::kRight: {
    return {tile.x + 1, tile.y};
  }

  default:
    return kInvalidTile;
  }
}

bool Snake::isValidTile(const Tile &tile) const {
  if (tile == kInvalidTile) {
    return false;
  }

  if (tile.x < 0 || tile.x >= fieldWidth_) {
    return false;
  }

  if (tile.y < 0 || tile.y >= fieldHeight_) {
    return false;
  }

  return true;
}

bool Snake::isEmptyTile(const Tile &tile) const {
  if (!isValidTile(tile)) {
    return false;
  }

  return world_->field[tile.x][tile.y].state == TileState::kEmpty;
}

bool Snake::isFoodTile(const Tile &tile) const {
  if (!isValidTile(tile)) {
    return false;
  }

  const auto &worldTile = world_->field[tile.x][tile.y];
  if (worldTile.state == TileState::kFood &&
      worldTile.occupiedBy == playerKind) {
    return true;
  }

  return false;
}
