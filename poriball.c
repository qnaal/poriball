#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>		// for atan2()
#include <SDL.h>
#include <SDL_image.h>

// macros
#define PI M_PI
#define GAME_SPEED 100.0	// updates/s
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_RADIUS 80
#define PLAYER_SPEED 500	// px/s

#define BALL_RADIUS 10
#define BALL_ACC 1000

#define KEY_QUIT SDLK_q

#define PORIMG "slime.png"

// structures
typedef struct {
  float x;
  float y;
} Pt;

typedef struct {
  float r;		   // should be positive when given the choice
  float theta;
} PtPol;

typedef struct {
  float depth;	   // must be positive
  float normal;	   // the direction the ball intersects into other obj
  Pt bvel;	   // ball velocity
  Pt ovel;	   // other obj velocity
} Contact;

typedef struct {
  // pos/size
  Pt pos;			// px
  int r;
  Pt vel;			// px/s
  // keys pressed
  bool pressl;
  bool pressr;
  // keys mapped
  SDLKey keyl;
  SDLKey keyr;
} Player;

typedef struct {
  //pos/size
  Pt pos;			// px
  int r;
  //vel
  Pt vel;			// px/s
} Ball;

typedef struct {
  bool running;
  Player p1;
  Ball b;
} World;

typedef struct {
  SDL_Surface *screen;
  SDL_Surface *porimg;
  Uint32 colfg;
  Uint32 colbg;
} GameData;


// prototypes
float time();
Contact make_contact();
bool init_video();
SDLKey wait_for_key();
SDLKey key_prompt(char subject[], char object[]);
Player make_player(float x, float y);
void move_player(Player *p, float physdt);
void draw_player(GameData *game, Player *p, SDL_Surface *img);
Ball spawn_ball(float x, float y);
void move_ball(Ball *b, float physdt);
void draw_ball(GameData *game, Ball *b);
float clamp(float x, float min, float max);
void draw_world(World *world, GameData *game);
void handle_events(World *world);
Pt vsum(Pt pt1, Pt pt2);
Pt vmlt(float s, Pt pt);
Pt vinv(Pt pt);
float vdot(Pt pt1, Pt pt2);
float pythag(Pt pt);
float azimuth(Pt pt);
PtPol polarize(Pt pt);
Pt carterize(PtPol pol);
Contact collision_wall(Ball *b);
Contact collision_player(Ball *b, Player *p);
void handle_collisions(World *w);
void physics(World *world, float physdt);

// functions
int main() {
  GameData game;
  init_video(&game);
  game.colfg = SDL_MapRGB(game.screen->format, 0x80, 0x80, 0x80);
  game.colbg = SDL_MapRGB(game.screen->format, 0xd0, 0xd0, 0xd0);
  game.porimg = IMG_Load(PORIMG); //200x100px, with the point 100,90 being the base point

  World world;
  world.running = true;
  world.p1 = make_player(100.0, 0.0);
  world.b = spawn_ball(100.0, 200.0);

  float phys_dt = 1/GAME_SPEED;
  float t0;
  float t1 = time();
  float phys_accum = 0.0;

  while(world.running) {

    t0 = t1;
    t1 = time();
    phys_accum += t1 - t0;

    while (phys_accum >= phys_dt) {
      handle_events(&world);
      phys_accum -= phys_dt;
      physics(&world, phys_dt);
    }

    draw_world(&world, &game);

  };
  return 0;
}

// returns time since init in seconds
float time() {
  return (float)SDL_GetTicks() / 1000;
}

Contact make_contact() {
  Contact c;
  c.depth = 0.0;
  c.normal = 0.0;
  c.bvel = (Pt){0.0,0.0};
  c.ovel = (Pt){0.0,0.0};
  return c;
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

SDLKey wait_for_key() {
  SDL_Event event;
  while (event.type != SDL_KEYDOWN)
    SDL_WaitEvent(&event);
  return event.key.keysym.sym;
}

SDLKey key_prompt(char subject[], char object[]) {
  printf("%s press %s\n", subject, object);
  SDLKey key = wait_for_key();
  printf("%s %s is %s\n", subject, object, SDL_GetKeyName(key));
  return key;
}

Player make_player(float x, float y) {
  Player p;
  p.pos = (Pt){x,y};
  p.r = PLAYER_RADIUS;
  p.vel = (Pt){0.0,0.0};
  p.pressl = false;
  p.pressr = false;

  p.keyl = key_prompt("player 1", "LEFT");
  p.keyr = key_prompt("player 1", "RIGHT");

  return p;
}

void move_player(Player *p, float dt) {
  int dir= 0;
  if (p->pressr) dir++;
  if (p->pressl) dir--;
  p->vel.x = dir * PLAYER_SPEED;
  p->pos.x = p->pos.x + p->vel.x * dt;
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

void move_ball(Ball *b, float dt) {
  b->pos = vsum( b->pos, vmlt( dt, b->vel));
  b->vel = vsum( b->vel,
		 vmlt( dt, (Pt){ 0.0, -BALL_ACC }));
}

void draw_ball(GameData *game, Ball *b) {
  int sq = b->r * 1.8;
  SDL_Rect rect = { b->pos.x - b->r, SCREEN_HEIGHT - (b->pos.y + b->r), sq, sq };
  SDL_FillRect( game->screen, &rect, game->colfg );
}

float clamp(float x, float min, float max) {
  if ( x < min )
    x = min;
  else if ( x > max )
    x = max;
  return x;
}

void draw_world(World *world, GameData *game) {
  SDL_FillRect(game->screen, NULL, game->colbg);
  draw_player(game, &world->p1, game->porimg);
  draw_ball(game, &world->b);
  SDL_Flip(game->screen);
}

void handle_events(World *world) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == world->p1.keyl) {
	world->p1.pressl = true;
      }
      else if (event.key.keysym.sym == world->p1.keyr) {
	world->p1.pressr = true;
      }
      else if (event.key.keysym.sym == KEY_QUIT) {
	world->running = false;
      }
      else
	printf("%s\n", SDL_GetKeyName(event.key.keysym.sym));
      break;
    case SDL_KEYUP:
      if (event.key.keysym.sym == world->p1.keyl) {
	world->p1.pressl = false;
      }
      else if (event.key.keysym.sym == world->p1.keyr) {
	world->p1.pressr = false;
      }
      break;
    case SDL_QUIT:
      world->running = false;
      break;
    }
  }
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

Pt vinv(Pt pt) {
  return (Pt){-pt.x,-pt.y};
}

// dot product
float vdot(Pt pt1, Pt pt2) {
  return pt1.x * pt2.x + pt1.y * pt2.y;
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
  float x = pol.r * cos(pol.theta);
  float y = pol.r * sin(pol.theta);
  return (Pt){x,y};
}

Contact collision_player(Ball *b, Player *p) {
  float mindist = b->r + p->r;
  Pt dif = vsum(p->pos, vmlt(-1, b->pos));
  PtPol dif_pol = polarize( dif );
  Contact contact = make_contact();
  if (dif_pol.r < mindist) {
    float dist =  dif_pol.r;
    contact.depth = mindist - dist;
    contact.normal = dif_pol.theta;
    contact.bvel = b->vel;
    contact.ovel = p->vel;
  }
  return contact;
}

Contact collision_wall(Ball *b) {
  float x = b->pos.x;
  float r = b->r;
  float w1 = 0;
  float w2 = SCREEN_WIDTH;
  Contact contact = make_contact();
  contact.bvel = b->vel;
  if ( x - r < w1 ) {
    contact.depth = r - x;
    contact.normal = PI;
  }
  else if ( x + r > w2 ) {
    contact.depth = x + r - w2;
    contact.normal = 0.0;
  }
  return contact;
}

void handle_collisions(World *w) {
  Ball *b = &w->b;
  Player *p = &w->p1;
  Contact contact = collision_wall(b);
  // FIXME: kind of messy
  if ( contact.depth == 0.0 )
    contact = collision_player(b, p);
  if (contact.depth != 0.0) {
    float dvelr = -2 * abs( vdot( vsum( contact.bvel, vinv(contact.ovel) ),
				  carterize( (PtPol){1.0, contact.normal} )
				  )
			    );
    Pt dvel = carterize( (PtPol){dvelr, contact.normal} );
    b->vel = vsum( b->vel, dvel );
  }
}

void physics(World *world, float dt) {
  move_player(&world->p1, dt);
  handle_collisions(world);
  move_ball(&world->b, dt);
}
