#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

class Scene;

union SDL_Event;

class SceneManager {
  std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
  std::string current;
  std::string next;

public:
  SceneManager();

  template <class TScene, class... TParam>
  TScene *Register(const std::string &name, TParam &&...params) {
    scenes.emplace(name,
                   std::make_unique<TScene>(std::forward<TParam>(params)...));
    return dynamic_cast<TScene *>(scenes[name].get());
  }

  void Change(const std::string &name);

  bool Update(const SDL_Event& event);

  bool Draw();

  std::string Current() const;
};

#endif /* SCENE_MANAGER_H */
