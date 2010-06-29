#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

// macros
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// prototypes
bool initVideo();

int main() {
  SDL_Event event;
  int running = true;

  initVideo();
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT ||
          (event.type == SDL_KEYDOWN &&
           event.key.keysym.sym == SDLK_q)||
          (event.type == SDL_KEYDOWN &&
           event.key.keysym.sym == SDLK_ESCAPE)
          )
	running = false;
    }
    SDL_Delay(30);
  };
  return 0;
}

bool initVideo() {

  SDL_Surface *screen;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return false;
  }
  atexit(SDL_Quit);

  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_ANYFORMAT);
  if (screen == NULL) {
    fprintf(stderr, "Unable to initialize video mode: %s\n", SDL_GetError());
    return false;
  }
  return true;
}
