#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "lib/scene_manager.h"
#include "lib/supa_scenes.h"
#include "miyoo.h"
#include "scenes/game_scene.h"

#include <sstream>

#include <iostream>

// Screen formatting
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BITS_PER_PIXEL = 32;

constexpr auto kTargetFrameRate = 60; // FPS
constexpr auto kTargetFrameTime = 1000.0f / kTargetFrameRate;

// Font formatting
const int FONT_SIZE = 64;

// Resource paths
const char *imagePath = "assets/img/battleback8.png";
const char *fontPath = "assets/font/MMXSNES.ttf";

const SDL_Color COLOR_WHITE = {255, 255, 255};
const SDL_Color COLOR_BLACK = {0, 0, 0};

int main(int argc, char **argv) {
  // init
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  SDL_Surface *video =
      SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, BITS_PER_PIXEL,
                       SDL_HWSURFACE | SDL_DOUBLEBUF);

  SDL_Surface *screen = SDL_CreateRGBSurface(
      SDL_HWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, BITS_PER_PIXEL, 0, 0, 0, 0);

  bool done = false;

  // load resources
  TTF_Font *font = TTF_OpenFont(fontPath, FONT_SIZE);

  SceneManager sm;

  sm.Register<ComboScene>("title")
      ->With<KeyAwaitScene>(&sm, miyoo::BTN_A, "game")
      ->With<TextureScene>(IMG_Load(imagePath), screen);

  sm.Register<GameScene>("game", &sm, screen);

  sm.Change("title");

  auto key = static_cast<SDLKey>(0);

  while (!done) {
    auto ts1 = SDL_GetTicks();

    // clear screen
    SDL_FillRect(screen, nullptr,
                 SDL_MapRGB(screen->format, COLOR_WHITE.r, COLOR_WHITE.g,
                            COLOR_WHITE.b));

    // poll for input
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        done = true;
      } else if (event.type == SDL_KEYDOWN) {
        key = event.key.keysym.sym;
        if (event.key.keysym.sym == miyoo::BTN_MENU) {
          done = true;
        }
      }
    }

    if (done) {
      std::cout << "quit" << std::endl;
    }

    sm.Update();

    sm.Draw();

    std::stringstream sstr;
    sstr << "Key " << key;

    SDL_Surface *text = TTF_RenderUTF8_Shaded(font, sstr.str().c_str(),
                                              COLOR_BLACK, COLOR_WHITE);

    // to center the text we need to know how wide the text is
    SDL_Rect textCentered = {(Sint16)((SCREEN_WIDTH - text->w) / 2),
                             (Sint16)((SCREEN_HEIGHT - text->h) / 2), 0, 0};
    // TTF_RenderUTF8_Shaded puts a nice outlined box around our text, but it
    // has too much height. This clips it down to a nice size.
    SDL_Rect textClipped = {0, 12, (Uint16)text->w, (Uint16)text->h};
    SDL_BlitSurface(text, &textClipped, screen, &textCentered);

    SDL_FreeSurface(text);

    // draw screen to vram
    SDL_BlitSurface(screen, nullptr, video, nullptr);
    SDL_Flip(video);

    auto ts2 = SDL_GetTicks();

    auto delta = ts2 - ts1;
    if (delta >= kTargetFrameTime) {
      std::cout << "Long run " << delta << std::endl;
    } else {
      auto sleepTime = kTargetFrameTime - delta;
      SDL_Delay(sleepTime);
    }
  }

  TTF_CloseFont(font);

  SDL_FreeSurface(screen);
  SDL_FreeSurface(video);

  // shutdown
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

  return 0;
}
