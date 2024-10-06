#ifndef GAME_FIELD_H
#define GAME_FIELD_H

#include <array>

#include "entities/enums.h"

constexpr auto kGameFieldWidth = 64; // tiles
constexpr auto kGameFieldHeight = 48;

struct GameTile {
  TileState state;
  PlayerKind occupiedBy;
};

using GameField =
    std::array<std::array<GameTile, kGameFieldHeight>, kGameFieldWidth>;

#endif /* GAME_FIELD_H */
