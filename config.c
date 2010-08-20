#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "gametypes.h"


static void parse_config_line(ConfigMatch *matches, char *line, bool verbose);


void read_config(ConfigMatch *matches) {

  char *dcfg[] = {
    /* world settings */
    "screenwidth = 640",    /* px */
    "screenheight = 480",
    "netheight = 150",
    "adtg = 1000", /* Acceleration Due To Gravity */

    /* player */
    "playerradius = 80",
    "playerspeed = 500",  /* px/s */
    "jumpvel = 400",
    "skywalk = false",

    /* ball */
    "ballradius = 10",
    "elasticity = .8", /* Elasticity of collisions, 1=fully elastic, 0=lame */

    /* display */
    "font = crap/ter-132b.pcf.gz",
    "fontsize = 32",
    "colorfg = 0x80 0x80 0x80",
    "colorbg = 0xd0 0xd0 0xd0",

    /* misc */
    "physhz = 200",  /* updates/s */
    "quitkey = 113",
    "dudes = 2",
    "porimg = crap/slime.png", /* 200x100px, with the point 100,90 being the base point */
    ""
  };
  bool verbose = false;
  if( verbose )
    puts("\nLoading Default Config:");
  char **l;
  for( l=dcfg; strcmp(*l,""); l++ ) {
    parse_config_line(matches, *l, verbose);
  }

  verbose = true;
  char configpath[] = "./config";
  if( verbose )
    printf("\nLoading Config File '%s':\n", configpath);
  FILE *config = fopen(configpath, "r");
  if( config ) {
    char line[BUFSIZ];
    while( fgets(line, sizeof(line), config) ) {
      parse_config_line(matches, line, verbose);
    }
    fclose(config);
    if( verbose )
      puts("end of config\n");
  } else
    if( verbose )
      puts(":< config file not found ;-;\n");
}

static void parse_config_line(ConfigMatch *matches, char *line, bool verbose) {
  char key[BUFSIZ], svalue[BUFSIZ];
  if( sscanf(line, "%[^#= ] = %[^\n]", key, svalue) != 2 )
    return;
  ConfigMatch *m;
  for( m=matches; strcmp(m->key,""); m++ ) {

    if( strcmp(m->key, key) == 0 ) {
      switch( m->type ) {
      case INT: {
	int value;
	sscanf(svalue, "%i", &value);
	*(int*)m->val = value;
	if( verbose )
	  printf("%s = %i\n", m->key, value);
	break;
      }
      case FLOAT: {
	float value;
	sscanf(svalue, "%f", &value);
	*(float*)m->val = value;
	if( verbose )
	  printf("%s = %f\n", m->key, value);
	break;
      }
      case COLOR: {
	int r,g,b;
	sscanf(svalue, "%i %i %i", &r,&g,&b);
	*(SDL_Color*)m->val = (SDL_Color){r,g,b};
	if( verbose )
	  printf("%s = %i,%i,%i\n", m->key, r,g,b);
	break;
      }
      case BOOL: {
	bool value = true;
	if( strcmp(svalue,"false") == 0 || strcmp(svalue,"0") == 0 )
	  value = false;
	*(bool*)m->val = value;
	if( verbose ) {
	  char *pr = "true";
	  if( !value )
	    pr = "false";
	  printf("%s = %s\n", m->key, pr);
	}
	break;
      }
      case STRING:
	strncpy((char*)m->val, svalue, STR_SHORT);
	if( verbose )
	  printf("%s = %s\n", m->key, (char*)m->val);
	break;
      }
    } /* if( strcmp() ) */

  } /* for( matches ) */
}
