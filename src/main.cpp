#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "lib/scene_manager.h"
#include "lib/supa_scenes.h"
#include "miyoo.h"
#include "scenes/game_scene.h"

// Screen formatting
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BITS_PER_PIXEL = 32;

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
  SDL_Surface *text =
      TTF_RenderUTF8_Shaded(font, "Hello world", COLOR_BLACK, COLOR_WHITE);

  SceneManager sm;

  sm.Register<ComboScene>("title")
      ->With<KeyAwaitScene>(&sm, miyoo::BTN_A, "game")
      ->With<TextureScene>(IMG_Load(imagePath), screen);

  sm.Register<GameScene>("game", &sm);

  sm.Change("title");

  while (!done) {
    // clear screen
    SDL_FillRect(screen, nullptr,
                 SDL_MapRGB(screen->format, COLOR_WHITE.r, COLOR_WHITE.g,
                            COLOR_WHITE.b));

    // poll for input
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      done = sm.Update(event);
    }

    sm.Draw();

    // to center the text we need to know how wide the text is
    SDL_Rect textCentered = {(Sint16)((SCREEN_WIDTH - text->w) / 2),
                             (Sint16)((SCREEN_HEIGHT - text->h) / 2), 0, 0};
    // TTF_RenderUTF8_Shaded puts a nice outlined box around our text, but it
    // has too much height. This clips it down to a nice size.
    SDL_Rect textClipped = {0, 12, (Uint16)text->w, (Uint16)text->h};
    SDL_BlitSurface(text, &textClipped, screen, &textCentered);

    // draw screen to vram
    SDL_BlitSurface(screen, nullptr, video, nullptr);
    SDL_Flip(video);
  }

  TTF_CloseFont(font);

  SDL_FreeSurface(text);
  SDL_FreeSurface(screen);
  SDL_FreeSurface(video);

  // shutdown
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

  return 0;
}
