#include "scene_manager.h"

#include "scene.h"

#include <iostream>

SceneManager::SceneManager() {}

void SceneManager::Change(const std::string &name) { next = name; }

bool SceneManager::Update(const SDL_Event& event) {
  if (current != next) {
    std::cout << "[INFO] change scene from '" << current << "' to '" << next
              << "'\n";

    if (scenes.count(current) != 0) {
      scenes[current]->Exit();
    }

    if (scenes.count(next) != 0) {
      scenes[next]->Activate();
    }

    current = next;
  }

  if (scenes.count(current) != 0) {
    scenes.at(current)->Update(event);
    return false;
  }

  return true;
}

bool SceneManager::Draw() {
  if (scenes.count(current) != 0) {
    scenes.at(current)->Draw();
    return true;
  }
  return false;
}

std::string SceneManager::Current() const { return current; }
