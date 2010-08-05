#ifndef gametypes_h
#define gametypes_h

#include <stdbool.h>
#include <SDL.h>
#include "macroconfig.h"
#include "vector.h"

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
  Pt pos;
  float theta;			// Angle of wall, 0 < theta < PI
} Wall;

typedef struct {
  bool running;
  unsigned pnum;		// number of players
  Player players[MAX_DUDES];
  unsigned wnum;
  Wall walls[5];
  Ball b;
} World;

typedef struct {
  SDL_Surface *screen;
  SDL_Surface *porimg;
  Uint32 colfg;
  Uint32 colbg;
} GameData;

#endif