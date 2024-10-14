#ifndef GAME_FIELD_H
#define GAME_FIELD_H

#include <array>

#include "entities/enums.h"

constexpr auto kTileSize = 16; // px

constexpr auto kGameFieldWidth = 40; // tiles
constexpr auto kGameFieldHeight = 30;
constexpr auto kGameFieldSize = kGameFieldHeight * kGameFieldWidth;

struct Tile {
  uint32_t x;
  uint32_t y;
};

struct GameTile {
  TileState state;
  PlayerKind occupiedBy;
};

using GameField =
    std::array<std::array<GameTile, kGameFieldHeight>, kGameFieldWidth>;

#endif /* GAME_FIELD_H */
