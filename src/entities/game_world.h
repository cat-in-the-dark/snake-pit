#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include "enums.h"
#include "food.h"
#include "game_field.h"
#include "snake.h"

#include <map>
#include <vector>

struct GameWorld {
  GameField field;
  std::map<PlayerKind, Snake> snakes;
  std::vector<Food> foods;
  int points;
};

#endif /* GAME_WORLD_H */
