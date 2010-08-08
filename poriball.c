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
Wall make_wall_long(Pt pos, float theta);
Wall make_wall(Pt pos, Pt pt2);
void players_key_prompt(GameData *game, World *w);
float clamp(float x, float min, float max);

/* functions */
int main() {
  srand(rtime());
  GameData game;
  init_video(&game);
  game.colfg = (SDL_Color){0x80, 0x80, 0x80};
  game.colbg = (SDL_Color){0xd0, 0xd0, 0xd0};
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
  players_key_prompt(&game, &world);

  world.wnum = 4;
  world.walls[0] = make_wall_long( (Pt){0,0}, M_PI/2 );
  world.walls[1] = make_wall_long( (Pt){SCREEN_WIDTH,0}, M_PI/2 );
  world.walls[2] = make_wall_long( (Pt){0,SCREEN_HEIGHT}, 0 ); /* RACQUETBALL */
  world.walls[3] = make_wall( (Pt){SCREEN_WIDTH/2, 0}, (Pt){0, NET_HEIGHT} );

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

Wall make_wall_long(Pt pos, float theta) {
  return (Wall){pos, line, theta};
}

Wall make_wall(Pt pt1, Pt pt2) {
  Wall w;
  w.pos = pt1;
  w.type = seg;
  w.pt2 = pt2;
  return w;
}

void players_key_prompt(GameData *game, World *w) {
  int i;
  Player *p;
  int len = 16;
  for( i=0;i < w->pnum;i++ ) {
    p = &w->players[i];
    char name[len];
    snprintf(name,len,"player %i",i);
    p->keyl = key_prompt(game, name, "LEFT");
    p->keyr = key_prompt(game, name, "RIGHT");
    p->keyj = key_prompt(game, name, "JUMP");
  }
}
