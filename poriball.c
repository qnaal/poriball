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

#define ADTG 1000		// Acceleration Due To Gravity

#define PLAYER_RADIUS 80
#define PLAYER_SPEED 500	// px/s
#define JUMP_VEL 400

#define BALL_RADIUS 10

#define KEY_QUIT SDLK_q

#define MAX_DUDES 2
#define MAX_CONTACTS 32

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
  bool pressj;
  // keys mapped
  SDLKey keyl;
  SDLKey keyr;
  SDLKey keyj;
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
  unsigned pnum;		// number of players
  Player players[MAX_DUDES];
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
void players_key_prompt(World *w);
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
  srand(time(NULL));
  GameData game;
  init_video(&game);
  game.colfg = SDL_MapRGB(game.screen->format, 0x80, 0x80, 0x80);
  game.colbg = SDL_MapRGB(game.screen->format, 0xd0, 0xd0, 0xd0);
  game.porimg = IMG_Load(PORIMG); //200x100px, with the point 100,90 being the base point

  World world;
  world.running = true;

  world.pnum = MAX_DUDES;
  Player *p;
  for (p = &world.players[0]; p < &world.players[world.pnum]; p++){
    float placement = PLAYER_RADIUS + ((float)rand() * (SCREEN_WIDTH - 2 * PLAYER_RADIUS)) / RAND_MAX;
    *p = make_player(placement,0);
  }
  players_key_prompt(&world);

  world.b = spawn_ball(world.players[0].pos.x, 200.0);

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

    if (world.b.pos.y < BALL_RADIUS)
      world.b = spawn_ball(world.players[0].pos.x, 200.0);

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
  printf("\n");	  // SDL chokes a fat one if I don't push two newlines
  return key;
}

Player make_player(float x, float y) {
  Player p;
  p.pos = (Pt){x,y};
  p.r = PLAYER_RADIUS;
  p.vel = (Pt){0.0,0.0};
  p.pressl = false;
  p.pressr = false;

  return p;
}

void players_key_prompt(World *w) {
  int i;
  Player *p;
  char name[16];		// how do normal people do this?
  for (i=0;i<w->pnum;i++){
    p = &w->players[i];
    sprintf(name,"player %i",i);
    p->keyl = key_prompt(name, "LEFT");
    p->keyr = key_prompt(name, "RIGHT");
    p->keyj = key_prompt(name, "JUMP");
  }
}

void move_player(Player *p, float dt) {
  if (p->pos.y < 0){
    p->pos.y = 0;
    p->vel.y = 0;
  }
  if (p->pos.y == 0){
    if (p->pressj)
      p->vel.y = JUMP_VEL;
    int dir= 0;
    if (p->pressr) dir++;
    if (p->pressl) dir--;
    p->vel.x = dir * PLAYER_SPEED;
  }
  else {
    p->vel = vsum( p->vel, vmlt(dt, (Pt){0,-ADTG}) );
  }
  p->pos = vsum( p->pos, vmlt(dt, p->vel) );
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
		 vmlt( dt, (Pt){ 0.0, -ADTG }));
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
  Player *p;
  for (p = &world->players[0]; p < &world->players[world->pnum]; p++)
    draw_player(game, p, game->porimg);
  draw_ball(game, &world->b);
  SDL_Flip(game->screen);
}

void handle_events(World *world) {
  SDL_Event event;
  Player *p;
  bool done = false;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_KEYDOWN:
      for (p = &world->players[0]; p < &world->players[world->pnum]; p++){
	if(event.key.keysym.sym == p->keyl){
	  p->pressl = true;
	  done = true;
	}
	else if(event.key.keysym.sym == p->keyr){
	  p->pressr = true;
	  done = true;
	}
	else if(event.key.keysym.sym == p->keyj){
	  p->pressj = true;
	  done = true;
	}
      }
      if (!done){
	if (event.key.keysym.sym == KEY_QUIT) {
	  world->running = false;
	}
	else
	  printf("%s\n", SDL_GetKeyName(event.key.keysym.sym));
      }
      break;
    case SDL_KEYUP:
      for (p = &world->players[0]; p < &world->players[world->pnum]; p++){
	if(event.key.keysym.sym == p->keyl){
	  p->pressl = false;
	  done = true;
	}
	else if(event.key.keysym.sym == p->keyr){
	  p->pressr = false;
	  done = true;
	}
	else if(event.key.keysym.sym == p->keyj){
	  p->pressj = false;
	  done = true;
	}
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
  Contact contacts[MAX_CONTACTS];
  int cntnum = 0;

  // Player contacts
  Player *p;
  for (p = &w->players[0]; p < &w->players[w->pnum]; p++){
    contacts[cntnum] = collision_player(b, p);
    if ( contacts[cntnum].depth != 0.0 )
      cntnum++;
  }

  // Wall contacts
  contacts[cntnum] = collision_wall(b);
  if ( contacts[cntnum].depth != 0.0 )
    cntnum++;

  // Use the largest contact for each ball
  if (cntnum>0){
    Contact *c;
    Contact *big;
    float big_size = 0;
    for (c = &contacts[0]; c < &contacts[cntnum]; c++){
      if (c->depth > big_size){
	big = c;
	big_size = c->depth;
      }
    }
    if (big->depth != 0.0) {
      float dvelr = -2 * abs( vdot( vsum( big->bvel, vinv(big->ovel) ),
				    carterize( (PtPol){1.0, big->normal} )
				    )
			      );
      Pt dvel = carterize( (PtPol){dvelr, big->normal} );
      b->vel = vsum( b->vel, dvel );
    }
  }
}

void physics(World *world, float dt) {
  int i;
  for (i=0;i<world->pnum;i++){
    move_player(&world->players[i], dt);
  }
  handle_collisions(world);
  move_ball(&world->b, dt);
}
