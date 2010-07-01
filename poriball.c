#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

// macros
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 20

// structures
typedef struct {
  int x;
  int y;
  int w;
  int h;
} Player;

typedef struct {
  SDL_Surface *screen;
  Uint32 colfg;
} GameData;


// prototypes
bool init_video();
Player make_player(int x, int y);
void draw_player(GameData *game, Player p);

// functions
int main() {
  int running = true;
  Player p1 = make_player(100, 0);
  GameData game;

  init_video(&game);
  game.colfg = SDL_MapRGB(game.screen->format, 0xff, 0xff, 0xff);
  SDL_Event event;

  while (running) {

    draw_player(&game, p1);
    SDL_Flip(game.screen);

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

bool init_video(GameData *game) {

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return false;
  }
  atexit(SDL_Quit);

  game->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_ANYFORMAT);
  if (game->screen == NULL) {
    fprintf(stderr, "Unable to initialize video mode: %s\n", SDL_GetError());
    return false;
  }
  return true;
}

Player make_player(int x, int y) {
  Player p;
  p.x = x;
  p.y = y;
  p.w = PLAYER_WIDTH;
  p.h = PLAYER_HEIGHT;
  return p;
}

void draw_player(GameData *game, Player p) {
  SDL_Rect rect = { p.x, SCREEN_HEIGHT - p.y - p.h, p.w, p.h };
  SDL_FillRect( game->screen, &rect, game->colfg );
}
