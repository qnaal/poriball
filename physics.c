#include <math.h>
#include "gametypes.h"
#include "macroconfig.h"
#include "physics.h"
#include "vector.h"


typedef struct {
  float depth;	/* must be positive */
  float normal;	/* the direction the ball intersects into other obj */
  Pt bvel;	/* ball velocity */
  Pt ovel;	/* other obj velocity */
} Contact;

static Contact zero_contact() {
  Contact c;
  c.depth = 0;
  c.normal = 0;
  c.bvel = (Pt){0,0};
  c.ovel = (Pt){0,0};
  return c;
}


static void move_ball(Ball *b, float physdt);
static void move_player(Player *p, float physdt);
static void handle_collisions(World *w);


void physics(World *world, float dt) {
  int i;
  for( i=0;i < world->pnum;i++ ) {
    move_player(&world->players[i], dt);
  }
  handle_collisions(world);
  move_ball(&world->b, dt);
}


static void move_ball(Ball *b, float dt) {
  b->pos = vsum( b->pos, vmlt(dt, b->vel) );
  b->vel = vsum( b->vel, vmlt(dt, (Pt){0,-ADTG}) );
}

static void move_player(Player *p, float dt) {
  if( p->pos.y < 0 ) {
    p->pos.y = 0;
    p->vel.y = 0;
  }
  if( p->pos.y == 0 || p->skywalk ) {
    int dir= 0;
    if( p->pressr ) dir++;
    if( p->pressl ) dir--;
    p->vel.x = dir * PLAYER_SPEED;
  }
  if( p->pos.y == 0 ) {
    if( p->pressj )
      p->vel.y = JUMP_VEL;
  } else {
    p->vel = vsum( p->vel, vmlt(dt, (Pt){0,-ADTG}) );
  }
  p->pos = vsum( p->pos, vmlt(dt, p->vel) );
}

static Contact collision_player(Ball *b, Player *p) {
  float mindist = b->r + p->r;
  Pt dif = vsum( p->pos, vmlt(-1, b->pos) );
  PtPol dif_pol = polarize( dif );
  Contact contact = zero_contact();
  if( dif_pol.r < mindist ) {
    float dist =  dif_pol.r;
    contact.depth = mindist - dist;
    contact.normal = dif_pol.theta;
    contact.bvel = b->vel;
    contact.ovel = p->vel;
  }
  return contact;
}

static Contact collision_wall(Ball *b, Wall *w) {
  Pt bpos = vdif( b->pos, w->pos ); /* relative position */
  /* close_r: distance along w from w.pos to the closest pt to b.pos */
  float close_r = cos(w->theta - azimuth(bpos)) * pythag(bpos);
  /* I LOVE BOTH OF THESE SO MUCH I CAN'T PICK JUST ONE */
  /* float close_r = vdot( carterize((PtPol){1,w->theta}), bpos ); */
  Pt close = carterize( (PtPol){close_r,w->theta} );
  Pt diff = vdif( close, bpos );
  float diff_r = pythag(diff);
  Contact contact = zero_contact();
  contact.depth = b->r - diff_r;
  if( contact.depth < 0 ) {
    contact.depth = 0;
  } else {
    contact.normal = azimuth(diff); /* This is always going to be the same as w->theta +/- PI/2 */
    contact.bvel = b->vel;
  }
  return contact;
}

static void handle_collisions(World *w) {
  static bool hitlast = true;
  Ball *b = &w->b;
  Contact contacts[MAX_CONTACTS];
  int cntnum = 0;

  /* Player contacts */
  Player *p;
  for( p = &w->players[0]; p < &w->players[w->pnum]; p++ ) {
    contacts[cntnum] = collision_player(b, p);
    if( contacts[cntnum].depth != 0 )
      cntnum++;
  }

  /* Wall contacts */
  Wall *wall;
  for( wall = &w->walls[0]; wall < &w->walls[w->wnum]; wall++ ) {
    contacts[cntnum] = collision_wall(b, wall);
    if( contacts[cntnum].depth != 0 ) {
      cntnum++;
    }
  }

  /* Use the largest contact for each ball */
  if( hitlast == false ) {
    if( cntnum > 0 ) {
      hitlast = true;
      Contact *c;
      Contact *big;
      float big_size = 0;
      for( c = &contacts[0]; c < &contacts[cntnum]; c++ ) {
	if( c->depth > big_size ) {
	  big = c;
	  big_size = c->depth;
	}
      }
      if( big->depth != 0 ) {
	/*
	 * FIXME: in the case that Ball is going in dir X at speed S,
	 * and Dude collides from behind, going in dir X at speed S+n,
	 * this logic fails. ...I think...
	 */
	float dvelr = -(1 + ELASTICITY) * abs( vdot( vdif( big->bvel, big->ovel ),
						     carterize( (PtPol){1, big->normal} )
						     )
					       );
	Pt dvel = carterize( (PtPol){dvelr, big->normal} );
	b->vel = vsum(b->vel, dvel);
      }
    }
  } else {
    hitlast = false;
  } /* hitlast==false */
}
