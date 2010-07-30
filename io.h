#include "gametypes.h"

float rtime();
bool init_video();
SDLKey wait_for_key();
SDLKey key_prompt(char subject[], char object[]);
void draw_ball(GameData *game, Ball *b);
void draw_player(GameData *game, Player *p, SDL_Surface *img);
void draw_world(World *world, GameData *game);
void handle_events(World *world);
