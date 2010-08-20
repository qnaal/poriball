#ifndef gametypes_h
#define gametypes_h

#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "vector.h"

#define MAX_DUDES 8
#define STR_SHORT 128

typedef struct {
  float l;
  float r;
} Territory;

typedef struct {
  /* pos/size */
  Pt pos;			 /* px */
  float r;
  Pt vel;			 /* px/s */
  Territory *reign;
  /* keys pressed */
  bool pressl;
  bool pressr;
  bool pressj;
  /* keys mapped */
  SDLKey keyl;
  SDLKey keyr;
  SDLKey keyj;
  /* misc */
  bool skywalk;
  float speed;			 /* max ground speed */
  float jumpvel;
} Player;

typedef struct {
  /* pos/size */
  Pt pos;			/* px */
  float r;
  /* vel */
  Pt vel;			/* px/s */
} Ball;

typedef struct {
  Pt pos;
  enum {
    SEG,
    LINE,
  } type;
  float theta;		 /* Angle of linewall; 0 < theta < PI */
  Pt pt2;		 /* pos of second pt of the seg, rel to pos */
} Wall;

/* default values to spawn dudes with */
typedef struct {
  float radius;
  float speed;
  float jumpvel;
  bool skywalk;
} PDefs;

typedef struct {
  bool running;
  unsigned pnum;		/* number of players */
  Player players[MAX_DUDES];
  PDefs pdefs;
  unsigned wnum;
  Wall walls[5];
  Ball b;
  float ballradius;
  float elasticity;
  unsigned tnum;
  Territory terras[2];
  float t0;
  float t1;
  int width;
  int height;
  float netheight;
  float physhz;
  float adtg;
} World;

typedef struct {
  World *world;
  SDL_Surface *screen;
  SDL_Surface *porimg;
  SDL_Color colfg;
  SDL_Color colbg;
  TTF_Font *font;
  char fontpath[STR_SHORT];
  int fontsize;
  SDLKey quitkey;
} GameData;

#endif
