/* GEOLIB.C -- useful functions for geographic stuff */


#ifndef GEOLIB_H__
#define GEOLIB_H__

#include "gflib.h"
#include <string.h>
#include <math.h>


#define VERYBIG  1.0E+12
#define VERYSMALL 1.0E-12
#define MAXXYZ 1000
#define LIMITXYZ 500

struct xyzdata
  {
   int count;
   double x[MAXXYZ];
   double y[MAXXYZ];
   double z[MAXXYZ];
  };

struct gridarray
  {
   int xgrids;
   int ygrids;
   double grid[90][90];
  };

struct scaledata
  {
   int iod;
   double wx0, wy0, wx1, wy1;
   double sx0, sy0, sx1, sy1;
   char wxtype, wytype, sxtype, sytype; /* compatible with geo_code, strgeo */
  };

struct pointlist
  {
   double x0;
   double y0;
   double x1;
   double y1;
   double z[4];
  };

struct linelist
  {
   int count;
   double x0[30];
   double y0[30];
   double x1[30];
   double y1[30];
  };

double solve3(int op, double d, double a, double b, double c);
double evalquad(double x, double a, double b, double c);

struct curvescaledata
  {
   double cpx[9];
   double cpy[9];

   double a[4];
   double b[4];
   double c[4];

   double ux0;  /* x0,y0   lower left user coordinates */
   double uy0;

   double ux1;  /* x1,y1   upper right user coordinates */
   double uy1;

   char xtype;
   char ytype;
  };

void cvtcurvescales(struct curvescaledata *cd, double *ix, double *iy,
                   double x, double y);
void solvecurves(struct curvescaledata *cd);
void convertscales(struct scaledata *scales, double *ix, double *iy, double x, double y);
void copyscales(struct scaledata *to, struct scaledata *from);
void invertscales(struct scaledata *to, struct scaledata *from);
void makerectangular(double *rx, double *ry, double ra, double rd);
void makepolar(double rx, double ry, double *ra, double *rd);
void rotatethru(double ra, double *rx, double *ry);

extern char geo_code;

double strgeo(char strg[], int pos, int zlen, char gcode);
char *geostr(double value, int dp, char gcode);
int xstrgeo(char strg[], int pos, int zlen, int dp, double value, char gcode);
void showscales(struct scaledata *scales);

#endif
