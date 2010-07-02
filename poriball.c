#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

// macros
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 20
#define PLAYER_SPEED 5

#define KEY_QUIT SDLK_q
#define KEY_P1_L SDLK_s
#define KEY_P1_R SDLK_f

// structures
typedef struct {
  // pos/size
  int x;
  int y;
  int w;
  int h;
  // keys
  bool l;
  bool r;
} Player;

typedef struct {
  SDL_Surface *screen;
  Uint32 colfg;
  Uint32 colbg;
} GameData;


// prototypes
bool init_video();
Player make_player(int x, int y);
void move_player(Player *p);
void draw_player(GameData *game, Player *p);

// functions
int main() {
  int running = true;
  Player p1 = make_player(100, 0);
  GameData game;

  init_video(&game);
  game.colfg = SDL_MapRGB(game.screen->format, 0xff, 0xff, 0xff);
  game.colbg = SDL_MapRGB(game.screen->format, 0x00, 0x00, 0x00);
  SDL_Event event;

  while(running) {

    SDL_FillRect(game.screen, NULL, game.colbg);
    draw_player(&game, &p1);
    SDL_Flip(game.screen);

    // handle events
    while(SDL_PollEvent(&event)) {
      switch(event.type){
      case SDL_KEYDOWN:
	switch(event.key.keysym.sym){
	case KEY_P1_L:
	  p1.l = true;
	  break;
	case KEY_P1_R:
	  p1.r = true;
	  break;
	case KEY_QUIT:
	  running = false;
	  break;
	default:
	  break;
	}
	break;
      case SDL_KEYUP:
	switch(event.key.keysym.sym){
	case KEY_P1_L:
	  p1.l = false;
	  break;
	case KEY_P1_R:
	  p1.r = false;
	  break;
	default:
	  break;
	}
	break;
      case SDL_QUIT:
	running = false;
	break;
      }

    } // while (events)

    move_player(&p1);

    SDL_Delay(10);
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
  p.l = false;
  p.r = false;
  return p;
}

void move_player(Player *p) {
  int dir= 0;
  if (p->r) dir++;
  if (p->l) dir--;
  p->x = p->x + dir * PLAYER_SPEED;
}

void draw_player(GameData *game, Player *p) {
  SDL_Rect rect = { p->x, SCREEN_HEIGHT - p->y - p->h, p->w, p->h };
  SDL_FillRect( game->screen, &rect, game->colfg );
}
