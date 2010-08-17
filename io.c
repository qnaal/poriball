#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_gfxPrimitives.h>
#include "gametypes.h"


/*
 * Event Handling
 */

typedef void (*EvH)(World*, SDL_Event*);

static EvH keymap[SDLK_LAST];

static void evh_quit(World *world, SDL_Event *event) {
  world->running = false;
}
static void evh_player(World *world, SDL_Event *event) {
  bool keydown;
  if( event->type == SDL_KEYDOWN )
    keydown = true;
  else keydown = false;
  Player *p;
  for( p = world->players; p < &world->players[world->pnum]; p++ ) {
    if( event->key.keysym.sym == p->keyl ) {
      p->pressl = keydown;
    }
    if( event->key.keysym.sym == p->keyr ) {
      p->pressr = keydown;
    }
    if( event->key.keysym.sym == p->keyj ) {
      p->pressj = keydown;
    }
  }
}

void handle_events(World *world) {
  SDL_Event event;
  EvH handler = 0;
  while( SDL_PollEvent(&event) ) {
    switch( event.type ) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      handler = keymap[event.key.keysym.sym];
      break;
    case SDL_QUIT:
      handler = &evh_quit;
      break;
    }
    if( handler )
      handler(world, &event);
  }
}


/*
 * The Other SDL Stuff
 */

static int screenheight;


/* screen-dependent pixel value */
static Uint32 map_color_pixel(SDL_PixelFormat *fmt, SDL_Color *c) {
  Uint32 color = SDL_MapRGB(fmt, c->r, c->g, c->b );
  return color;
}
/* 0xRRGGBBAA */
static Uint32 map_color_gfx(SDL_Color *c) {
  return ((Uint32) c->r << 24) | ((Uint32) c->g << 16) | ((Uint32) c->b << 8) | ((Uint32) 255);
}

/* returns the projected pixel location of pt */
static Pt project_scr(Pt pt) {
  return (Pt){pt.x, screenheight - pt.y};
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
  if( (game->font = TTF_OpenFont(game->fontpath,game->fontsize)) == NULL ) {
    fprintf(stderr, "Unable to load font: %s\n", game->fontpath);
  }

  game->screen = SDL_SetVideoMode(game->world->width, game->world->height, 0, SDL_ANYFORMAT);
  if( game->screen == NULL ) {
    fprintf(stderr, "Unable to initialize video mode: %s\n", SDL_GetError());
    return false;
  }

  screenheight = game->world->height;
  keymap[game->quitkey] = &evh_quit;

  return true;
}

static SDLKey wait_for_key() {
  SDL_Event event;
  while (event.type != SDL_KEYDOWN)
    SDL_WaitEvent(&event);
  return event.key.keysym.sym;
}

static char msg[STR_SHORT];

SDLKey key_prompt(GameData *game, char subject[], char object[]) {
  sprintf(msg, "%s press %s", subject, object);

  SDL_FillRect( game->screen, NULL, map_color_pixel(game->screen->format, &game->colbg) );
  SDL_Surface *msg_surface = TTF_RenderText_Solid( game->font, msg, game->colfg );
  SDL_BlitSurface( msg_surface, NULL, game->screen, NULL );
  SDL_Flip(game->screen);
  msg[0] = '\0';

  while( true ) {
    SDLKey key = wait_for_key();
    if( keymap[key] == NULL ) {
      keymap[key] = &evh_player;
      printf("%s %s is %s\t(%i)\n", subject, object, SDL_GetKeyName(key), key);
      return key;
    } else puts("Key already assigned");
  }
}

static void fps_counter(char *fps, int t0, int t1) {
  static int lastcheck = 0;
  static int fps_accum = 0;
  if( floor(t1) > lastcheck ) {
    lastcheck = floor(t1);
    float dt = t1 - t0;
    if( dt > 0 )
      sprintf( fps, "%i fps", fps_accum );
    fps_accum = 0;
  } else
    fps_accum++;
}

static void draw_ball(GameData *game, Ball *b) {
  Pt scrpos = project_scr(b->pos);
  filledCircleColor( game->screen, scrpos.x, scrpos.y, b->r, map_color_gfx(&game->colfg) );
}

static void draw_player(GameData *game, Player *p, SDL_Surface *img) {
  Pt imgcorner = {-100,90};
  Pt destpt = project_scr( vsum(p->pos,imgcorner) );
  SDL_Rect dest = { destpt.x,destpt.y, 0,0 };
  SDL_BlitSurface( img, NULL, game->screen, &dest );
}

static void draw_wall(GameData *game, Wall *w) {
  if( w->type == seg ) {
    Pt pt1 = project_scr( w->pos);
    Pt pt2 = project_scr( vsum(w->pt2,w->pos) );
    aalineColor( game->screen, pt1.x, pt1.y, pt2.x, pt2.y, map_color_gfx(&game->colfg) );
  }
  /* FIXME: should draw lines too */
}

void draw_world(World *world, GameData *game) {
  SDL_FillRect( game->screen, NULL, map_color_pixel(game->screen->format, &game->colbg) );

  Wall *w;
  for( w = world->walls; w < &world->walls[world->wnum]; w++ )
    draw_wall(game, w);

  Player *p;
  for( p = world->players; p < &world->players[world->pnum]; p++ )
    draw_player(game, p, game->porimg);

  draw_ball(game, &world->b);

  fps_counter(msg, world->t0, world->t1);

  SDL_Surface *msg_surface = TTF_RenderText_Solid( game->font, msg, game->colfg );
  SDL_BlitSurface( msg_surface, NULL, game->screen, NULL );
  SDL_Flip(game->screen);
}
