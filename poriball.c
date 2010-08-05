#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "gametypes.h"
#include "io.h"
#include "macroconfig.h"
#include "physics.h"

/* prototypes */
Ball make_ball(float x, float y);
Player make_player(float x, float y);
Wall make_wall(Pt pos, float theta);
void players_key_prompt(World *w);
float clamp(float x, float min, float max);

/* functions */
int main() {
  srand(rtime());
  GameData game;
  init_video(&game);
  game.colfg = SDL_MapRGB(game.screen->format, 0x80, 0x80, 0x80);
  game.colbg = SDL_MapRGB(game.screen->format, 0xd0, 0xd0, 0xd0);
  game.porimg = IMG_Load(PORIMG); /* 200x100px, with the point 100,90 being the base point */

  World world;
  world.running = true;

  world.pnum = MAX_DUDES;
  Player *p;
  for( p = &world.players[0]; p < &world.players[world.pnum]; p++ ) {
    float placement = PLAYER_RADIUS + ( (float)rand() * (SCREEN_WIDTH - 2 * PLAYER_RADIUS) ) / RAND_MAX;
    *p = make_player(placement,0);
    p->skywalk = SKYWALK;
  }
  players_key_prompt(&world);

  world.wnum = 3;
  world.walls[0] = make_wall( (Pt){0,0}, M_PI/2 );
  world.walls[1] = make_wall( (Pt){SCREEN_WIDTH,0}, M_PI/2 );
  world.walls[2] = make_wall( (Pt){0,SCREEN_HEIGHT}, 0 ); /* RACQUETBALL */

  world.b = make_ball(world.players[0].pos.x, 200);

  float phys_dt = 1/GAME_SPEED;
  float t0;
  float t1 = rtime();
  float phys_accum = 0;

  while( world.running ) {

    t0 = t1;
    t1 = rtime();
    phys_accum += t1 - t0;

    while( phys_accum >= phys_dt ) {
      handle_events(&world);
      phys_accum -= phys_dt;
      physics(&world, phys_dt);
    }

    draw_world(&world, &game);

    if( world.b.pos.y < BALL_RADIUS )
      world.b = make_ball(world.players[0].pos.x, 200);

  };
  return 0;
}

Ball make_ball(float x, float y) {
  Ball b;
  b.pos = (Pt){x,y};
  b.r = BALL_RADIUS;
  b.vel = (Pt){0,0};
  return b;
}

Player make_player(float x, float y) {
  Player p;
  p.pos = (Pt){x,y};
  p.r = PLAYER_RADIUS;
  p.vel = (Pt){0,0};
  p.pressl = false;
  p.pressr = false;

  return p;
}

Wall make_wall(Pt pos, float theta) {
  return (Wall){pos, theta};
}

void players_key_prompt(World *w) {
  int i;
  Player *p;
  char name[16];		/* how do normal people do this? */
  for( i=0;i < w->pnum;i++ ) {
    p = &w->players[i];
    sprintf(name,"player %i",i);
    p->keyl = key_prompt(name, "LEFT");
    p->keyr = key_prompt(name, "RIGHT");
    p->keyj = key_prompt(name, "JUMP");
  }
}

float clamp(float x, float min, float max) {
  if( x < min )
    x = min;
  else if( x > max )
    x = max;
  return x;
}
