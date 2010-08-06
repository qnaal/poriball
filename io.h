#include "gametypes.h"

float rtime();
bool init_video();
SDLKey key_prompt(char subject[], char object[]);
void draw_world(World *world, GameData *game);
void handle_events(World *world);
