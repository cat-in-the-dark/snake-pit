#ifndef SUPA_SCENE_H
#define SUPA_SCENE_H

#include <memory>
#include <string>
#include <vector>

#include <SDL/SDL_keysym.h>
#include <SDL/SDL_video.h>

#include "lib/tasks/timer.h"
#include "scene.h"

class SceneManager;

class ComboScene : public Scene {
  std::vector<std::unique_ptr<Scene>> scenes;

public:
  ComboScene();

  void Activate();

  void Update(const SDL_Event &event);

  void Draw();

  void Exit();

  template <class TScene, class... TParam>
  ComboScene *With(TParam &&...params) {
    scenes.emplace_back(
        std::make_unique<TScene>(std::forward<TParam>(params)...));
    return this;
  }
};

class TimerScene : public Scene {
  SceneManager *sm;
  Timer timer;
  std::string next;
  uint32_t ts;

public:
  TimerScene(SceneManager *sm, float seconds, std::string next);

  void Activate();

  void Update(const SDL_Event &event);

  void Draw();

  void Exit();
};

class KeyAwaitScene : public Scene {
  SceneManager *sm;
  SDLKey key;
  std::string next;

public:
  KeyAwaitScene(SceneManager *sm, SDLKey key, std::string next);

  void Activate();

  void Update(const SDL_Event &event);

  void Draw();

  void Exit();
};

class TextureScene : public Scene {
  SDL_Surface *texture;
  SDL_Surface *screen;

public:
  TextureScene(SDL_Surface *texture, SDL_Surface *screen);

  ~TextureScene();

  void Activate();

  void Update(const SDL_Event &event);

  void Draw();

  void Exit();
};

#endif /* SUPA_SCENE_H */
