#ifndef FOOD_H
#define FOOD_H

#include "enums.h"
#include "game_field.h"

struct Food {
  Tile coords;
  PlayerKind kind;
};

#endif /* FOOD_H */
