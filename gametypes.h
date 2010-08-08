#ifndef gametypes_h
#define gametypes_h

#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "macroconfig.h"
#include "vector.h"

typedef struct {
  /* pos/size */
  Pt pos;			 /* px */
  int r;
  Pt vel;			 /* px/s */
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
} Player;

typedef struct {
  /* pos/size */
  Pt pos;			/* px */
  int r;
  /* vel */
  Pt vel;			/* px/s */
} Ball;

typedef struct {
  Pt pos;
  enum {
    seg,
    line
  } type;
  float theta;		 /* Angle of linewall; 0 < theta < PI */
  Pt pt2;		 /* pos of second pt of the seg, rel to pos */
} Wall;

typedef struct {
  bool running;
  unsigned pnum;		/* number of players */
  Player players[MAX_DUDES];
  unsigned wnum;
  Wall walls[5];
  Ball b;
} World;

typedef struct {
  SDL_Surface *screen;
  SDL_Surface *porimg;
  SDL_Color colfg;
  SDL_Color colbg;
  TTF_Font *font;
} GameData;

#endif
