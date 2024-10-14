#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <SDL/SDL_keysym.h>
#include <SDL/SDL_video.h>

#include <map>
#include <memory>
#include <vector>

#include "lib/scene.h"
#include "lib/scene_manager.h"
#include "lib/tasks/timer.h"
#include "lib/tasks/predicate.h"

#include "entities/game_world.h"
#include "entities/snake.h"

class GameScene : public Scene {
public:
  GameScene(SceneManager *sm, SDL_Surface *screen);

  virtual void Activate() override;

  virtual void Exit() override;

  virtual void Update() override;

  virtual void Draw() override;

private:
  SceneManager *sm_;

  std::map<PlayerKind, SDL_Surface *> tile_textures_;

  std::map<SDLKey, PlayerKind> activationKeyMap_;

  SDL_Surface *screen_;

  Timer timer_;

  uint32_t ts_;

  GameWorld world_;

  std::vector<PredicateTask> tasks_;

  PlayerKind currentPlayer_;

  Tile getRandomFreeTile() const;

  bool spawnFood(PlayerKind kind);

  bool spawnSnake(PlayerKind kind);
};

#endif /* GAME_SCENE_H */
