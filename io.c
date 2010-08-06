#include <SDL.h>
#include <SDL_ttf.h>
#include "gametypes.h"


static Uint32 map_color(SDL_PixelFormat *fmt, SDL_Color *c) {
  Uint32 color = SDL_MapRGB(fmt, c->r, c->g, c->b );
  return color;
}

/* returns time since init, in seconds */
float rtime() {
  return (float)SDL_GetTicks() / 1000;
}

bool init_video(GameData *game) {

  if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return false;
  }
  atexit(SDL_Quit);

  if( TTF_Init() == -1 ) {
    fprintf(stderr, "Unable to initialize SDL_ttf: %s\n", SDL_GetError());
    return false;
  }
  if( (game->font = TTF_OpenFont(MSG_FONT,MSG_SIZE)) == NULL ) {
    fprintf(stderr, "Unable to load font: %s\n", MSG_FONT);
  }

  game->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_ANYFORMAT);
  if( game->screen == NULL ) {
    fprintf(stderr, "Unable to initialize video mode: %s\n", SDL_GetError());
    return false;
  }
  return true;
}

static SDLKey wait_for_key() {
  SDL_Event event;
  while (event.type != SDL_KEYDOWN)
    SDL_WaitEvent(&event);
  return event.key.keysym.sym;
}

static char msg[128];

SDLKey key_prompt(GameData *game, char subject[], char object[]) {
  sprintf(msg, "%s press %s", subject, object);

  SDL_FillRect( game->screen, NULL, map_color(game->screen->format, &game->colbg) );
  SDL_Surface *msg_surface = TTF_RenderText_Solid( game->font, msg, game->colfg );
  SDL_BlitSurface( msg_surface, NULL, game->screen, NULL );
  SDL_Flip(game->screen);
  msg[0] = '\0';

  SDLKey key = wait_for_key();
  printf("%s %s is %s\n", subject, object, SDL_GetKeyName(key));
  return key;
}

static void draw_ball(GameData *game, Ball *b) {
  int sq = b->r * 1.8;
  SDL_Rect rect = { b->pos.x - b->r, SCREEN_HEIGHT - (b->pos.y + b->r), sq, sq };
  SDL_FillRect( game->screen, &rect, map_color(game->screen->format, &game->colfg) );
}

static void draw_player(GameData *game, Player *p, SDL_Surface *img) {
  SDL_Rect dest = { p->pos.x - 100, SCREEN_HEIGHT - (p->pos.y + 90), 0, 0 };
  SDL_BlitSurface( img, NULL, game->screen, &dest );
}

void draw_world(World *world, GameData *game) {
  SDL_FillRect( game->screen, NULL, map_color(game->screen->format, &game->colbg) );
  Player *p;
  for( p = &world->players[0]; p < &world->players[world->pnum]; p++ )
    draw_player(game, p, game->porimg);
  draw_ball(game, &world->b);
  SDL_Surface *msg_surface = TTF_RenderText_Blended( game->font, msg, game->colfg );
  SDL_BlitSurface( msg_surface, NULL, game->screen, NULL );
  SDL_Flip(game->screen);
}

void handle_events(World *world) {
  SDL_Event event;
  Player *p;
  bool done = false;
  while( SDL_PollEvent(&event) ) {
    switch(event.type) {
    case SDL_KEYDOWN:
      for( p = &world->players[0]; p < &world->players[world->pnum]; p++ ) {
	if( event.key.keysym.sym == p->keyl ) {
	  p->pressl = true;
	  done = true;
	} else if( event.key.keysym.sym == p->keyr ) {
	  p->pressr = true;
	  done = true;
	} else if( event.key.keysym.sym == p->keyj ) {
	  p->pressj = true;
	  done = true;
	}
      }
      if( !done ) {
	if( event.key.keysym.sym == KEY_QUIT ) {
	  world->running = false;
	} else
	  printf("%s\n", SDL_GetKeyName(event.key.keysym.sym));
      }
      break;
    case SDL_KEYUP:
      for( p = &world->players[0]; p < &world->players[world->pnum]; p++ ) {
	if( event.key.keysym.sym == p->keyl ) {
	  p->pressl = false;
	  done = true;
	} else if( event.key.keysym.sym == p->keyr ) {
	  p->pressr = false;
	  done = true;
	} else if( event.key.keysym.sym == p->keyj ) {
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
