#include "gametypes.h"

typedef struct {
  char key[STR_SHORT];
  void *val;
  enum {
    FLOAT,
    INT,
    COLOR,
    BOOL,
    STRING,
  } type;
} ConfigMatch;

void read_config(ConfigMatch *matches);
