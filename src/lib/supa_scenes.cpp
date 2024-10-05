#include "supa_scenes.h"

#include "scene_manager.h"

#include <SDL/SDL_events.h>
#include <SDL/SDL_timer.h>

ComboScene::ComboScene() {}
void ComboScene::Activate() {
  for (auto &scene : scenes) {
    scene->Activate();
  }
}
void ComboScene::Update() {
  for (auto &scene : scenes) {
    scene->Update();
  }
}
void ComboScene::Draw() {
  for (auto &scene : scenes) {
    scene->Draw();
  }
}
void ComboScene::Exit() {
  for (auto &scene : scenes) {
    scene->Exit();
  }
}

KeyAwaitScene::KeyAwaitScene(SceneManager *sm, SDLKey key, std::string next)
    : sm(sm), key(key), next(next) {}

void KeyAwaitScene::Activate() {}

void KeyAwaitScene::Update() {
  auto *keyState = SDL_GetKeyState(nullptr);
  if (keyState[key]) {
    sm->Change(next);
  }
}

void KeyAwaitScene::Draw() {}

void KeyAwaitScene::Exit() {}

TextureScene::TextureScene(SDL_Surface *texture, SDL_Surface *screen)
    : texture(texture), screen(screen) {}

TextureScene::~TextureScene() {
  if (texture != nullptr) {
    SDL_FreeSurface(texture);
    texture = nullptr;
  }
}

void TextureScene::Activate() {}

void TextureScene::Update() {}

void TextureScene::Draw() {
  SDL_BlitSurface(texture, nullptr, screen, nullptr);
}

void TextureScene::Exit() {}

TimerScene::TimerScene(SceneManager *sm, float seconds, std::string next)
    : ts{SDL_GetTicks()}, sm{sm}, timer{seconds}, next{next} {}

void TimerScene::Activate() { timer.Reset(); }

void TimerScene::Update() {
  auto ts1 = SDL_GetTicks();
  timer.Update(static_cast<float>(ts1 - ts));
  ts = ts1;

  if (timer.IsPassed()) {
    sm->Change(next);
  }
}

void TimerScene::Draw() {}

void TimerScene::Exit() {}
