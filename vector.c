#include <math.h>
#include "vector.h"

Pt vsum(Pt pt1, Pt pt2) {
  Pt sum = { pt1.x + pt2.x, pt1.y + pt2.y };
  return sum;
}

Pt vdif(Pt pt1, Pt pt2) {
  return vsum(pt1, vinv(pt2));
}

/* scalar multiplication */
Pt vmlt(float s, Pt pt) {
  float x = s * pt.x;
  float y = s * pt.y;
  Pt product = {x,y};
  return product;
}

Pt vinv(Pt pt) {
  return (Pt){-pt.x,-pt.y};
}

/* dot product */
float vdot(Pt pt1, Pt pt2) {
  return pt1.x * pt2.x + pt1.y * pt2.y;
}

float pythag(Pt pt) {
  return sqrt( pow(pt.x, 2) + pow(pt.y, 2));
}

float azimuth(Pt pt) {
  double theta = atan2( pt.y, pt.x );
  return (float)theta;
}

PtPol polarize(Pt pt) {
  float r = pythag(pt);
  float theta = azimuth(pt);
  PtPol pol = {r, theta};
  return pol;
}

Pt carterize(PtPol pol) {
  float x = pol.r * cos(pol.theta);
  float y = pol.r * sin(pol.theta);
  return (Pt){x,y};
}
