#include "gametypes.h"

typedef struct {
  char key[STR_SHORT];
  void *val;
  enum {
    fl,				/* float */
    in,				/* int */
    co,				/* color */
    bo,				/* bool */
    st,				/* string */
  } type;
} ConfigMatch;

void read_config(ConfigMatch *matches);
