#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>		// for atan2()
#include <SDL.h>
#include <SDL_image.h>

// macros
#define GAME_SPEED 100.0	// updates/s
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_RADIUS 80
#define PLAYER_SPEED 500	// px/s

#define BALL_RADIUS 10
#define BALL_ACC 1000

#define KEY_QUIT SDLK_q
#define KEY_P1_L SDLK_s
#define KEY_P1_R SDLK_f

#define PORIMG "slime.png"

// structures
typedef struct {
  float x;
  float y;
} Pt;

typedef struct {
  float r;
  float theta;
} PtPol;

typedef struct {
  // pos/size
  Pt pos;			// px
  int r;
  // keys
  bool kl;
  bool kr;
} Player;

typedef struct {
  //pos/size
  Pt pos;			// px
  int r;
  //vel
  Pt vel;			// px/s
} Ball;

typedef struct {
  SDL_Surface *screen;
  Uint32 colfg;
  Uint32 colbg;
} GameData;


// prototypes
bool init_video();
Player make_player(float x, float y);
void move_player(Player *p);
void draw_player(GameData *game, Player *p, SDL_Surface *img);
Ball spawn_ball(float x, float y);
void move_ball(Ball *b);
void draw_ball(GameData *game, Ball *b);
Pt vsum(Pt pt1, Pt pt2);
Pt vmlt(float s, Pt pt);
float pythag(Pt pt);
float azimuth(Pt pt);
PtPol polarize(Pt pt);
Pt carterize(PtPol pol);
PtPol collision(Ball *b, Player *p);
void handle_collisions(Ball *b, Player *p);

// functions
int main() {
  int running = true;
  Player p1 = make_player(100.0, 0.0);
  Ball b = spawn_ball(100.0, 200.0);
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
    handle_collisions(&b, &p1);
    move_ball(&b);

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

Player make_player(float x, float y) {
  Player p;
  p.pos = (Pt){x,y};
  p.r = PLAYER_RADIUS;
  p.kl = false;
  p.kr = false;
  return p;
}

void move_player(Player *p) {
  int dir= 0;
  if (p->kr) dir++;
  if (p->kl) dir--;
  p->pos.x = p->pos.x + dir * PLAYER_SPEED / GAME_SPEED;
}

void draw_player(GameData *game, Player *p, SDL_Surface *img) {
  SDL_Rect dest = { p->pos.x - 100, SCREEN_HEIGHT - (p->pos.y + 90), 0, 0 };
  SDL_BlitSurface( img, NULL, game->screen, &dest );
}

Ball spawn_ball(float x, float y) {
  Ball b;
  b.pos = (Pt){x,y};
  b.r = BALL_RADIUS;
  b.vel = (Pt){ 0.0, 0.0 };
  return b;
}

void move_ball(Ball *b) {
  b->pos = vsum( b->pos, vmlt( 1 / GAME_SPEED, b->vel));
  b->vel = vsum( b->vel,
		 vmlt( 1.0 / GAME_SPEED, (Pt){ 0.0, -BALL_ACC }));
}

void draw_ball(GameData *game, Ball *b) {
  int sq = b->r * 1.8;
  SDL_Rect rect = { b->pos.x - b->r, SCREEN_HEIGHT - (b->pos.y + b->r), sq, sq };
  SDL_FillRect( game->screen, &rect, game->colfg );
}

Pt vsum(Pt pt1, Pt pt2) {
  Pt sum = { pt1.x + pt2.x, pt1.y + pt2.y };
  return sum;
}

// scalar multiplication
Pt vmlt(float s, Pt pt) {
  float x = s * pt.x;
  float y = s * pt.y;
  Pt product = {x,y};
  return product;
}

float pythag(Pt pt) {
  return sqrt( pow(pt.x, 2) + pow(pt.y, 2));
}

float azimuth(Pt pt) {
  double theta = atan2( pt.y, pt.x );
  return (float)theta;
}

PtPol polarize(Pt pt) {
  float r = pythag(pt);
  float theta = azimuth(pt);
  PtPol pol = {r, theta};
  return pol;
}

Pt carterize(PtPol pol) {
  float x = pol.r * sin(pol.theta);
  float y = pol.r * cos(pol.theta);
  return (Pt){x,y};
}

PtPol collision(Ball *b, Player *p) {
  float mindist = b->r + p->r;
  Pt dif = vsum(b->pos, vmlt(-1, p->pos));
  PtPol dif_pol = polarize( dif );
  if (dif_pol.r < mindist) {
    float dist =  dif_pol.theta;
    PtPol contact_pol = { dist - mindist, dif_pol.theta };
    return contact_pol;
  }
  return (PtPol){0.0, 0.0};
}

void handle_collisions(Ball *b, Player *p) {
  PtPol contact = collision(b, p);
  if (contact.r != 0.0) {
    b->vel.y = -b->vel.y;
  }
}
