#include <iostream>

#include "game_scene.h"

#include <SDL/SDL_events.h>

#include <SDL/SDL_image.h>

GameScene::GameScene(SceneManager *sm, SDL_Surface *screen)
    : sm_(sm), screen_(screen), tile_tex_(IMG_Load("assets/img/tile.png")) {}

void GameScene::Activate() {}

void GameScene::Exit() {}

void GameScene::Update() {}

void GameScene::Draw() {
  SDL_Rect textCentered{};
  textCentered.w = kTileSize;
  textCentered.h = kTileSize;

  for (size_t y = 0; y < kGameFieldHeight; y++) {
    for (size_t x = 0; x < kGameFieldWidth; x++) {
      textCentered.x = x * kTileSize;
      textCentered.y = y * kTileSize;

      SDL_BlitSurface(tile_tex_.get(), nullptr, screen_, &textCentered);
    }
  }
}
