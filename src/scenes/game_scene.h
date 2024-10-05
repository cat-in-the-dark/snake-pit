#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <SDL/SDL_video.h>

#include <memory>

#include "lib/scene.h"
#include "lib/scene_manager.h"

constexpr auto kGameFieldWidth = 64; // tiles
constexpr auto kGameFieldHeight = 48;

constexpr auto kTileSize = 10; // px

enum class TileState { kEmpty, kP1 };

class GameScene : public Scene {
public:
  GameScene(SceneManager *sm, SDL_Surface *screen);

  virtual void Activate() override;

  virtual void Exit() override;

  virtual void Update() override;

  virtual void Draw() override;

private:
  SceneManager *sm_;

  std::unique_ptr<SDL_Surface> tile_tex_;

  SDL_Surface *screen_;

  TileState gameFeild_[kGameFieldWidth][kGameFieldHeight];
};

#endif /* GAME_SCENE_H */
