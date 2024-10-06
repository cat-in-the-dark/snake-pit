#include "snake.h"

#include <iostream>

Snake::Snake(uint32_t fieldWidth, uint32_t fieldHeight, PlayerKind playerKind,
             GameWorld *world)
    : fieldWidth_(fieldWidth), fieldHeight_(fieldHeight),
      playerKind(playerKind), tiles{}, world_(world) {}

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

void Snake::setDirection(Direction dir) { currentDir_ = dir; }

bool Snake::move() {
  auto next = nextTile(tiles.front(), currentDir_);
  if (!isValidTile(next)) {
    return false;
  }

  tiles.push_front(next);
  return true;
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

  if (world_->field[tile.x][tile.y].state != TileState::kEmpty) {
    std::cout << "kek" << std::endl;
    return false;
  }

  return true;
}
