#ifndef SNAKE_H
#define SNAKE_H

#include <cstddef>
#include <cstdint>
#include <deque>

#include "game_field.h"

struct GameWorld;

enum class Direction { kUp, kDown, kLeft, kRight };

enum class MoveResult { kSuccess, kCollision, kFoodTaken };

constexpr uint32_t kInvalidCoord = static_cast<uint32_t>(-1);

inline bool operator==(const Tile &lhs, const Tile &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

constexpr auto kInvalidTile = Tile{kInvalidCoord, kInvalidCoord};

class Snake {
public:
  std::deque<Tile> tiles;

  PlayerKind playerKind;

  Snake(uint32_t fieldWidth, uint32_t fieldHeight, PlayerKind playerKind,
        GameWorld *world);

  bool spawn(uint32_t startX, uint32_t startY, Direction startDirection,
             size_t initLength);

  void moveTail();

  void setDirection(Direction dir);

  Direction getDirection() const;

  void activate();

  bool isPlayable() const;

  MoveResult move();

private:
  Tile nextTile(const Tile &tile, Direction dir);

  bool isValidTile(const Tile &tile) const;

  bool isEmptyTile(const Tile& tile) const;

  bool isFoodTile(const Tile &tile) const;

  bool isPlayable_;

  uint32_t fieldWidth_;

  uint32_t fieldHeight_;

  Direction currentDir_;

  GameWorld *world_;

  bool canSetDirection_;
};

#endif /* SNAKE_H */
