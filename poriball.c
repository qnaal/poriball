#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>

// macros
#define GAME_SPEED 100		// updates/s
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_RADIUS 80
#define PLAYER_SPEED 500	// px/s

#define BALL_RADIUS 10

#define KEY_QUIT SDLK_q
#define KEY_P1_L SDLK_s
#define KEY_P1_R SDLK_f

#define PORIMG "slime.png"

// structures
typedef struct {
  // pos/size
  int x;
  int y;
  int r;
  // keys
  bool kl;
  bool kr;
} Player;

typedef struct {
  //pos/size
  int x;
  int y;
  int r;
  //vel
  int vx;			// px/s
  int vy;
} Ball;

typedef struct {
  SDL_Surface *screen;
  Uint32 colfg;
  Uint32 colbg;
} GameData;


// prototypes
bool init_video();
Player make_player(int x, int y);
void move_player(Player *p);
void draw_player(GameData *game, Player *p, SDL_Surface *img);
Ball spawn_ball(int x, int y);
void draw_ball(GameData *game, Ball *b);

// functions
int main() {
  int running = true;
  Player p1 = make_player(100, 0);
  Ball b = spawn_ball(100, 200);
  GameData game;

  SDL_Surface *porimg = IMG_Load(PORIMG); //200x100px, with the point 100,90 being the base point

  init_video(&game);
  game.colfg = SDL_MapRGB(game.screen->format, 0x80, 0x80, 0x80);
  game.colbg = SDL_MapRGB(game.screen->format, 0xd0, 0xd0, 0xd0);
  SDL_Event event;

  while(running) {

    SDL_FillRect(game.screen, NULL, game.colbg);
    draw_player(&game, &p1, porimg);
    draw_ball(&game, &b);
    SDL_Flip(game.screen);

    // handle events
    while(SDL_PollEvent(&event)) {
      switch(event.type){
      case SDL_KEYDOWN:
	switch(event.key.keysym.sym){
	case KEY_P1_L:
	  p1.kl = true;
	  break;
	case KEY_P1_R:
	  p1.kr = true;
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
	  p1.kl = false;
	  break;
	case KEY_P1_R:
	  p1.kr = false;
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

    SDL_Delay(1000 / GAME_SPEED);
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
  p.r = PLAYER_RADIUS;
  p.kl = false;
  p.kr = false;
  return p;
}

void move_player(Player *p) {
  int dir= 0;
  if (p->kr) dir++;
  if (p->kl) dir--;
  p->x = p->x + dir * PLAYER_SPEED / GAME_SPEED;
}

void draw_player(GameData *game, Player *p, SDL_Surface *img) {
  SDL_Rect dest = { p->x - 100, SCREEN_HEIGHT - (p->y + 90), 0, 0 };
  SDL_BlitSurface( img, NULL, game->screen, &dest );
}

Ball spawn_ball(int x, int y) {
  Ball b;
  b.x = x;
  b.y = y;
  b.r = BALL_RADIUS;
  b.vx = 0;
  b.vy = 0;
  return b;
}

void draw_ball(GameData *game, Ball *b) {
  int sq = b->r * 1.8;
  SDL_Rect rect = { b->x - b->r, SCREEN_HEIGHT - (b->y + b->r), sq, sq };
  SDL_FillRect( game->screen, &rect, game->colfg );
}
