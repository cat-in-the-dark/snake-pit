#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <SDL/SDL_video.h>

#include <memory>
#include <vector>

#include "lib/scene.h"
#include "lib/scene_manager.h"
#include "lib/tasks/timer.h"

#include "entities/game_world.h"
#include "entities/snake.h"

constexpr auto kTileSize = 10; // px

class GameScene : public Scene {
public:
  GameScene(SceneManager *sm, SDL_Surface *screen);

  virtual void Activate() override;

  virtual void Exit() override;

  virtual void Update() override;

  virtual void Draw() override;

private:
  std::vector<Snake> snakes;

  SceneManager *sm_;

  std::unique_ptr<SDL_Surface> tile_tex_;

  SDL_Surface *screen_;

  Timer timer_;

  uint32_t ts_;

  GameWorld world_;
};

#endif /* GAME_SCENE_H */
