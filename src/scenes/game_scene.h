#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "lib/scene.h"
#include "lib/scene_manager.h"

class GameScene : public Scene {
public:
  GameScene(SceneManager *sm) : sm_(sm) {}

  virtual void Activate() override;

  virtual void Exit() override;

  virtual void Update(const SDL_Event &event) override;

  virtual void Draw() override;

private:
  SceneManager *sm_;
};

#endif /* GAME_SCENE_H */
