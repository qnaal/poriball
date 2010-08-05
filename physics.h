#include "vector.h"
#include "gametypes.h"


typedef struct {
  float depth;	/* must be positive */
  float normal;	/* the direction the ball intersects into other obj */
  Pt bvel;	/* ball velocity */
  Pt ovel;	/* other obj velocity */
} Contact;


void move_ball(Ball *b, float physdt);
void move_player(Player *p, float physdt);
Contact zero_contact();
Contact collision_player(Ball *b, Player *p);
Contact collision_wall(Ball *b, Wall *w);
void handle_collisions(World *w);
void physics(World *world, float physdt);
