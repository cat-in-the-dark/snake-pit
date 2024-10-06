#ifndef SNAKE_H
#define SNAKE_H

#include <cstddef>
#include <cstdint>
#include <deque>

#include "game_world.h"

enum class Direction { kUp, kDown, kLeft, kRight };

constexpr uint32_t kInvalidCoord = static_cast<uint32_t>(-1);

struct Tile {
  uint32_t x;
  uint32_t y;
};

inline bool operator==(const Tile &lhs, const Tile &rhs) {
  return lhs.x == rhs.x && lhs.x == rhs.y;
}

constexpr auto kInvalidTile = Tile{kInvalidCoord, kInvalidCoord};

class Snake {
public:
  std::deque<Tile> tiles;

  PlayerKind playerKind;

  Snake(uint32_t fieldWidth, uint32_t fieldHeight, PlayerKind playerKind,
        GameWorld* field);

  bool spawn(uint32_t startX, uint32_t startY, Direction startDirection,
             size_t initLength);

  void moveTail();

  void setDirection(Direction dir);

  bool move();

private:
  Tile nextTile(const Tile &tile, Direction dir);

  bool isValidTile(const Tile &tile) const;

  uint32_t fieldWidth_;

  uint32_t fieldHeight_;

  Direction currentDir_;

  GameWorld* world_;
};

#endif /* SNAKE_H */
