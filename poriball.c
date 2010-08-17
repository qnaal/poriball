#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "config.h"
#include "gametypes.h"
#include "io.h"
#include "physics.h"


float rand1();
Ball make_ball(float x, float y, World *world);
Player make_player(Territory *reign, PDefs *pdefs);
Wall make_wall_long(Pt pos, float theta);
Wall make_wall(Pt pos, Pt pt2);
void players_key_prompt(GameData *game);
float clamp(float x, float min, float max);


int main() {
  puts("gotime");
  World world;
  world.running = true;
  GameData game;
  game.world = &world;

  {
    float adtg;
    char porimgpath[STR_SHORT];
    ConfigMatch matches[] = {
      {"screenwidth", &world.width, in},
      {"screenheight", &world.height, in},
      {"netheight", &world.netheight, fl},
      {"adtg", &adtg, fl},
      {"playerradius", &world.pdefs.radius, fl},
      {"playerspeed", &world.pdefs.speed, fl},
      {"jumpvel", &world.pdefs.jumpvel, fl},
      {"skywalk", &world.pdefs.skywalk, bo},
      {"ballradius", &world.ballradius, fl},
      {"elasticity", &world.elasticity, fl},
      {"font", &game.fontpath, st},
      {"fontsize", &game.fontsize, in},
      {"colorfg", &game.colfg, co},
      {"colorbg", &game.colbg, co},
      {"physhz", &world.physhz, fl},
      {"quitkey", &game.quitkey, in},
      {"dudes", &world.pnum, in},
      {"porimg", &porimgpath, st},
      {""}
    };
    read_config(matches);

    turn_on_gravity(adtg); /* never forget! */
    game.porimg = IMG_Load(porimgpath);
  }

  init_video(&game);

  world.wnum = 4;
  world.walls[0] = make_wall_long( (Pt){0,0}, M_PI/2 );
  world.walls[1] = make_wall_long( (Pt){world.width,0}, M_PI/2 );
  world.walls[2] = make_wall_long( (Pt){0,world.height}, 0 ); /* RACQUETBALL */
  world.walls[3] = make_wall( (Pt){world.width/2, 0}, (Pt){0, world.netheight} );

  world.tnum = 2;
  world.terras[0] = (Territory){2*world.ballradius, world.width/2 - 2*world.ballradius};
  world.terras[1] = (Territory){world.width/2 + 2*world.ballradius, world.width - 2*world.ballradius};

  Player *p;
  for( p = world.players; p < &world.players[world.pnum]; p++ ) {
    Territory *reign = &world.terras[(p - world.players) % world.tnum];
    *p = make_player(reign, &world.pdefs);
  }
  world.b = make_ball(world.players[0].pos.x, 200, &world);
  players_key_prompt(&game);

  float phys_dt = 1/world.physhz;
  world.t1 = rtime();
  float phys_accum = 0;

  srand(rtime());
  while( world.running ) {

    world.t0 = world.t1;
    world.t1 = rtime();
    phys_accum += world.t1 - world.t0;

    while( phys_accum >= phys_dt ) {
      handle_events(&world);
      phys_accum -= phys_dt;
      physics(&world, phys_dt);
    }

    draw_world(&world, &game);

    if( world.b.pos.y < world.ballradius ) {
      int spawnon = floor(rand1()*world.pnum);
      world.b = make_ball(world.players[spawnon].pos.x, 200, &world);
    }

  };
  return 0;
}

float rand1(int max) {
  return (float)rand()/RAND_MAX;
}

Ball make_ball(float x, float y, World *world) {
  Ball b;
  b.pos = (Pt){x,y};
  b.r = world->ballradius;
  b.vel = (Pt){0,0};
  return b;
}

Player make_player(Territory *reign, PDefs *pdefs) {
  Player p;
  p.r = pdefs->radius;
  p.speed = pdefs->speed;
  p.jumpvel = pdefs->jumpvel;
  p.skywalk = pdefs->skywalk;
  p.reign = reign;
  p.pos = (Pt){(reign->l+reign->r)/2, 0};
  p.vel = (Pt){0,0};
  p.pressl = false;
  p.pressr = false;
  p.pressj = false;
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

void players_key_prompt(GameData *game) {
  World *w = game->world;
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
