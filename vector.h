#ifndef vector_h
#define vector_h

typedef struct {
  float x;
  float y;
} Pt;

typedef struct {
  float r;		/* should be positive when given the choice */
  float theta;
} PtPol;

#endif

float clamp(float x, float min, float max);
Pt vsum(Pt pt1, Pt pt2);
Pt vdif(Pt pt1, Pt pt2);
Pt vmlt(float s, Pt pt);
Pt vinv(Pt pt);
float vdot(Pt pt1, Pt pt2);
float pythag(Pt pt);
float azimuth(Pt pt);
PtPol polarize(Pt pt);
Pt carterize(PtPol pol);
