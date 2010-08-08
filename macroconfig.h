#define GAME_SPEED 200.0	/* updates/s */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define NET_HEIGHT 150

#define MSG_FONT "crap/ter-132b.pcf.gz"
#define MSG_SIZE 32

#define ADTG 1000		/* Acceleration Due To Gravity */

#define PLAYER_RADIUS 80
#define PLAYER_SPEED 500	/* px/s */
#define JUMP_VEL 400
#define SKYWALK false

#define BALL_RADIUS 10
#define ELASTICITY 0.8 /* Elasticity of collisions, 1=fully elastic, 0=lame */

#define KEY_QUIT SDLK_q

#define MAX_CONTACTS 32
#define MAX_DUDES 2 /* FIXME: make World.players a resizable array? */

#define PORIMG "crap/slime.png"
