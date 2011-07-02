/* GEOLIB.C -- useful functions for geographic stuff */


#include "geolib.h"


static char tstrg[128];


double solve3(int op, double d, double a, double b, double c)

  {
   static double m[3][3];
   static double d0,d1,d2;
   static double f1,f2,f3,f4,f5,f6,f7,f8,f9;
   static double denom;
   static double x;
   static double y;
   static double z;
   static int n;
   static int calc = 0;
   int i;
   int j;

   if (op < 0)
     {
      for (i=0; i<3; i++)
        {
         for (j=0; j<3; j++)
           {
            m[i][j] = 0.0;
           }
        }
      n = 0;
      calc = 0;
      return 0.0;
     }
   if (op == 0)
     {
      switch (n)
        {
         case 0:
           {
            d0 = d;
            break;
           }
         case 1:
           {
            d1 = d;
            break;
           }
         case 2:
           {
            d2 = d;
            break;
           }
         default:
           {
            return 0.0;
           }
        }
      m[n][0] = a;
      m[n][1] = b;
      m[n][2] = c;
      n++;
      return 0.0;
     }
   if (!calc)
     {
      f1 = m[1][1] * m[2][2]  -   m[2][1] * m[1][2];
      f2 = m[2][1] * m[0][2]  -   m[0][1] * m[2][2];
      f3 = m[0][1] * m[1][2]  -   m[1][1] * m[0][2];
      f4 = m[2][0] * m[1][2]  -   m[1][0] * m[2][2];
      f5 = m[0][0] * m[2][2]  -   m[2][0] * m[0][2];
      f6 = m[1][0] * m[0][2]  -   m[0][0] * m[1][2];
      f7 = m[1][0] * m[2][1]  -   m[2][0] * m[1][1];
      f8 = m[2][0] * m[0][1]  -   m[0][0] * m[2][1];
      f9 = m[0][0] * m[1][1]  -   m[1][0] * m[0][1];

      denom = m[0][0] * f1  +   m[1][0] * f2   +  m[2][0] * f3;
      x = +(d0 * f1  +   d1 * f2   +   d2 * f3)  / denom;
      y = +(d0 * f4  +   d1 * f5   +   d2 * f6)  / denom;
      z = +(d0 * f7  +   d1 * f8   +   d2 * f9)  / denom;

      calc = 1;
     }
   if (op == 1)
     {
      return x;
     }
   if (op == 2)
     {
      return y;
     }
   return z;
  }


double evalquad(double x, double a, double b, double c)

  {
   return a * x * x  +  b * x  +  c;
  }




void cvtcurvescales(struct curvescaledata *cd, double *ix, double *iy,
                   double x, double y)

   /* convert x & y coordinates to ix & iy scaled coordinates using a method which
      adapts to small amounts of projection curvature in x & y, forcing rectilinear
      coordinates for ix & iy -- adequate for accurate digitizing of large and
      medium-scale maps */

  {
   double tx;
   double ty;

       /* Note: assumes:
     (ux0,uy1)^       (ux1,uy1)
              |           (direction of x or y coordinate scale dosn't matter)
              |
              y  x ------>
     (ux0,uy0)        (ux1,uy0)
       */

   tx = evalquad(y,cd->a[2],cd->b[2],cd->c[2]);
   ty = evalquad(x,cd->a[3],cd->b[3],cd->c[3]);

   *ix = +((x-tx) * (cd->ux1 - cd->ux0))
              / (evalquad(y,cd->a[0],cd->b[0],cd->c[0]) - tx)
                 + cd->ux0;
   *iy = +((y-ty) * (cd->uy1 - cd->uy0))
              / (evalquad(x,cd->a[1],cd->b[1],cd->c[1]) - ty)
                 + cd->uy0;
  }



void solvecurves(struct curvescaledata *cd)

  {
   /* solve for equation of right curve correction  dig x = f(dig y) */
   solve3(-1,1,1,1,1);
   solve3(0,cd->cpx[2],cd->cpy[2] * cd->cpy[2], cd->cpy[2], 1);
   solve3(0,cd->cpx[5],cd->cpy[5] * cd->cpy[5], cd->cpy[5], 1);
   solve3(0,cd->cpx[8],cd->cpy[8] * cd->cpy[8], cd->cpy[8], 1);

   cd->a[0] = solve3(1,1,1,1,1);
   cd->b[0] = solve3(2,1,1,1,1);
   cd->c[0] = solve3(3,1,1,1,1);

   /* solve for equation of top curve correction  dig y = f(dig x) */
   solve3(-1,1,1,1,1);
   solve3(0,cd->cpy[0],cd->cpx[0] * cd->cpx[0], cd->cpx[0], 1);
   solve3(0,cd->cpy[1],cd->cpx[1] * cd->cpx[1], cd->cpx[1], 1);
   solve3(0,cd->cpy[2],cd->cpx[2] * cd->cpx[2], cd->cpx[2], 1);

   cd->a[1] = solve3(1,1,1,1,1);
   cd->b[1] = solve3(2,1,1,1,1);
   cd->c[1] = solve3(3,1,1,1,1);

   /* solve for equation of left curve correction  dig x = f(dig y) */
   solve3(-1,1,1,1,1);
   solve3(0,cd->cpx[0],cd->cpy[0] * cd->cpy[0], cd->cpy[0], 1);
   solve3(0,cd->cpx[3],cd->cpy[3] * cd->cpy[3], cd->cpy[3], 1);
   solve3(0,cd->cpx[6],cd->cpy[6] * cd->cpy[6], cd->cpy[6], 1);

   cd->a[2] = solve3(1,1,1,1,1);
   cd->b[2] = solve3(2,1,1,1,1);
   cd->c[2] = solve3(3,1,1,1,1);

   /* solve for equation of bottom curve correction  dig y = f(dig x) */
   solve3(-1,1,1,1,1);
   solve3(0,cd->cpy[6],cd->cpx[6] * cd->cpx[6], cd->cpx[6], 1);
   solve3(0,cd->cpy[7],cd->cpx[7] * cd->cpx[7], cd->cpx[7], 1);
   solve3(0,cd->cpy[8],cd->cpx[8] * cd->cpx[8], cd->cpx[8], 1);

   cd->a[3] = solve3(1,1,1,1,1);
   cd->b[3] = solve3(2,1,1,1,1);
   cd->c[3] = solve3(3,1,1,1,1);
  }



void convertscales(struct scaledata *scales, double *ix, double *iy, double x, double y)

   /* *******************************************************************
      CONVERT REAL, SCALED X & Y TO REAL SCREEN COORDINATES IX & IY
      ACTUAL SCREEN WINDOW IS DEFINED BY INTEGRAL WX0,WY0 TO WX1,WY1
      SCALING AND VECTORING DETERMINED BY MAPPING REAL SX0,SY0 TO SX1,SY1
      ONTO SCREEN WINDOW SPECIFIED USING A RECTILINEAR METHOD

      Note: convert x,y in S coordinates ---> *ix, *iy in W coordinates

      ******************************************************************* */

  {
   double ax0, ay0, ax1, ay1;
   double ax, ay;
   double axdiff, aydiff, wxdiff, wydiff;
   double mxvert, myvert, bxvert, byvert;
   double rx, ry;

   /* convert sx, sy to ax, ay for orientation direction "iod" */

   switch (scales->iod)
     {
      case 2:     /* orientation 2 */
        {
         ax0 = scales->sy1;
         ay0 = scales->sx0;
         ax1 = scales->sy0;
         ay1 = scales->sx1;
         ax = y;
         ay = x;
         break;
        }
      case 3:     /* orientation 3 */
        {
         ax0 = scales->sx1;
         ay0 = scales->sy1;
         ax1 = scales->sx0;
         ay1 = scales->sy0;
         ax = x;
         ay = y;
         break;
        }
      case 4:     /* orientation 4 */
        {
         ax0 = scales->sy0;
         ay0 = scales->sx1;
         ax1 = scales->sy1;
         ay1 = scales->sx0;
         ax = y;
         ay = x;
         break;
        }
      default:    /*  orientation 1 */
        {
         ax0 = scales->sx0;
         ay0 = scales->sy0;
         ax1 = scales->sx1;
         ay1 = scales->sy1;
         ax = x;
         ay = y;
        }
     }

   /* determine actual pixel coordinates */

   wxdiff = scales->wx1 - scales->wx0;
   wydiff = scales->wy1 - scales->wy0;
   axdiff = ax1 - ax0;
   aydiff = ay1 - ay0;

   /* GET SLOPES & INTERCEPTS OF TRANSLATION FUNCTIONS */

   mxvert = wxdiff / axdiff;
   bxvert = scales->wx1 - ( mxvert * ax1 );
   myvert = wydiff / aydiff;
   byvert = scales->wy1 - ( myvert * ay1 );
   *ix = ( ax * mxvert ) + bxvert;
   *iy = ( ay * myvert ) + byvert;
  }



void copyscales(struct scaledata *to, struct scaledata *from)

  {
   to->iod = from->iod;
   to->wx0 = from->wx0;
   to->wy0 = from->wy0;
   to->wx1 = from->wx1;
   to->wy1 = from->wy1;
   to->sx0 = from->sx0;
   to->sy0 = from->sy0;
   to->sx1 = from->sx1;
   to->sy1 = from->sy1;
   to->wxtype = from->wxtype;
   to->wytype = from->wytype;
   to->sxtype = from->sxtype;
   to->sytype = from->sytype;
  }


void invertscales(struct scaledata *to, struct scaledata *from)

  {
   double t;
   char tchr;

   to->iod = from->iod;

   t = from->wx0;         /* in case to and from are the same entity */
   to->wx0 = from->sx0;
   to->sx0 = t;

   t = from->wy0;
   to->wy0 = from->sy0;
   to->sy0 = t;

   t = from->wx1;
   to->wx1 = from->sx1;
   to->sx1 = t;

   t = from->wy1;
   to->wy1 = from->sy1;
   to->sy1 = t;

   tchr = from->wxtype;
   to->wxtype = from->sxtype;
   to->sxtype = tchr;

   tchr = from->wytype;
   to->wytype = from->sytype;
   to->sytype = tchr;
  }


void makerectangular(double *rx, double *ry, double ra, double rd)

  {
   double absx;
   double absy;
   double ztan;

   *rx = rd * (cos(ra / 57.29577951));
   *ry = rd * (sin(ra / 57.29577951));
  }



void makepolar(double rx, double ry, double *ra, double *rd)

  {
   double absx;
   double absy;
   double ztan;

   *rd = sqrt(rx * rx + ry * ry);
   absx = fabs(rx);
   absy = fabs(ry);
   if (absx > absy)
     {
      if (absx > 0.0000001)
         ztan = atan(ry / absx) * 57.29577951;
      else
        {
         if (ry < -0.0000001)
            ztan = 270.0;
         else
            ztan = 90.0;
        }
      if (rx < -0.0000001)
        {
         *ra = 180.0 - ztan;
        }
      else
        {
         *ra = ztan;
        }
     }
   else
     {
      if (absy > 0.0000001)
         ztan = atan(rx / absy) * 57.29577951;
      else
         if (rx > 0.0000001)
            ztan = 0.0;
         else
            ztan = 180.0;
      if (ry > 0.0000001)
        {
         *ra = 90.0 - ztan;
        }
      else
        {
         *ra = ztan - 90;
        }
     }
   if (*ra < 0.0)
      *ra = *ra + 360.0;
  }



void rotatethru(double ra, double *rx, double *ry)

  {
   double ha;
   double hd;

   makepolar(*rx,*ry,&ha,&hd);
   ha = ha + ra;
   if (ha > 360.0)
      ha = ha - 360.0;
   makerectangular(rx,ry,ha,hd);
  }



char geo_code;

static char geo_buffer[256];

double strgeo(char strg[], int pos, int zlen, char gcode)
  {
   int geo_pos;
   int bpos;
   int hpos;
   double value;
   int len;
   char strnew[128];
   int flag;
   int i;
   int neg;
   int deg;
   int min;
   int sec;
   int hour;
   double rla;
   double rlb;
   double rlc;
   int done;
   int times;
   int err;
   double tval;
   int glen;


   if (zlen > 255)
      zlen = 255;
   if (zlen <-255)
      zlen = -255;

   if (pos < 0)
      pos = 0;
   glen = zlen;
   if (glen < 0)
      glen = -glen;
   hpos = pos;
   strs(geo_buffer,strg,pos,glen);
   bpos = strpos;

   done = FALSE;
   geo_code = 0;
   switch(gcode)
     {
      case 'D': ;
      case 'd': ;
      case 'x': ;
      case 'X': ;
      case '^': ;
      case 'ø':         /* legal symbols */
        {
         geo_code = 'D';
         break;
        }
      case 'h': ;
      case 'H':
        {
         geo_code = 'H';
         break;
        }
      default:
        {
         geo_code = 0;
        }
     }
   value = 0.0;
   rla = 0.0;
   rlb = 0.0;
   rlc = 0.0;
   neg = FALSE;
   err = FALSE;
   times = 0;
   geo_pos = 0;
   do
     {
      len = strlen(geo_buffer);
      strcpy(strnew,"");
      times++;
      i = geo_pos;
      flag = FALSE;
      while ((i < len ) && (geo_buffer[i] == ' '))
        {
         i++;
        }
      do
        {
         deg = FALSE;
         min = FALSE;
         sec = FALSE;
         hour = FALSE;
         if (i >= len)
           {
            flag = TRUE;
           }
         else
           {
            switch (geo_buffer[i])
              {
               case ',':
                 {
                  flag = TRUE;
                  break;
                 }
               case '-':
                 {
                  neg = TRUE;
                  break;
                 }
               case '.':
                 {
                  strcat(strnew,tostring(geo_buffer[i]));
                  break;
                 }
               case ' ':
                 {
                  flag = TRUE;
                  i--;
                  break;
                 }
               case 'D': ;
               case 'X': ;
               case 'd': ;
               case 'x': ;
               case '^': ;
               case 'ø':         /* degree symbol */
                 {
                  deg = TRUE;
                  flag = TRUE;
                  geo_code = 'D';
                  break;
                 }
               case '\x27':      /* minute of arc symbol */
                 {
                  min = TRUE;
                  flag = TRUE;
                  geo_code = 'D';
                  break;
                 }
               case '"':         /* second of arc symbol */
                 {
                  sec = TRUE;
                  flag = TRUE;
                  geo_code = 'D';
                  break;
                 }
               case 'h': ;
               case 'H':         /* hour of right ascension symbol */
                 {
                  hour = TRUE;
                  geo_code = 'H';
                  flag = TRUE;
                  break;
                 }
               case 'm': ;
               case 'M':         /* minute of right ascension symbol */
                 {
                  min = TRUE;
                  flag = TRUE;
                  geo_code = 'H';
                  break;
                 }
               case 's': ;
               case 'S':         /* second of right ascension symbol */
                 {
                  sec = TRUE;
                  flag = TRUE;
                  geo_code = 'H';
                  break;
                 }
               case '0': ;
               case '1': ;
               case '2': ;
               case '3': ;
               case '4': ;
               case '5': ;
               case '6': ;
               case '7': ;
               case '8': ;
               case '9':
                 {
                  strcat(strnew,tostring(geo_buffer[i]));
                  break;
                 }
               default:
                 {
                  err = TRUE;
                  done = TRUE;
                 }
              }
            i++;
           }
        }
      while (!flag);
      tval = strd(strnew,0,0);
      if (!geo_code)
        {
         rla = tval;
         done = TRUE;
        }
      if (deg)
        {
         rla = tval;
        }
      if (hour)
        {
         rla = tval;
        }
      if (min)
        {
         rlb = tval;
        }
      if (sec)
        {
         rlc = tval;
        }
      geo_pos = i;
      if (err)
        {
         bpos = hpos;
         done = TRUE;
        }
      if (times >= 3)
         done = TRUE;
      if ((i >= len) && (!done))
        {
         if (glen == 0)
           {
            hpos = bpos;
            strs(geo_buffer,strg,bpos,0);
            bpos = strpos;
            geo_pos = 0;
           }
         else
           {
            done = TRUE;
           }
        }
     }
   while (!done);

   tval = rla + ( rlb / 60.0 ) + ( rlc / 3600.0 );
   if (geo_code == 'H')
      tval = tval * 15.0;
   value = tval;
   if (neg)
      value = - value;
   strpos = bpos;
   return value;
  }



char *geostr(double value, int dp, char gcode)

  {
   static double errtab[] =
     {
      0.5, 0.05, 0.005, 0.0005, 0.00005, 0.000005,
      0.0000005, 0.00000005, 0.000000005, 0.0000000005
     };

   char wkstrg[40];
   double hold;
   int neg;
   double rla;
   double rlb;
   double rlc;
   int i;
   int len;
   int siz;
   char kar;
   int dpflag;


   siz = 0;
   hold = value;
   neg = FALSE;
   if (dp < 0)
      dp = 0;
   switch (gcode)
     {
      case 'D': ;
      case 'X': ;
      case 'd': ;
      case 'x': ;
      case '^': ;
      case 'ø':     /* degrees , minutes , seconds */
        {
         if (dp == 0)
            siz = 2;
         else
            siz = dp + 3;
         if (dp < 0)
            dp = 0;
         if (hold < 0.0)
           {
            neg = TRUE;
            hold = - hold;
           }
         rla = floor(hold);
         rlb = (hold - rla) * 60.0;
         rlc = (rlb - floor(rlb)) * 60.0;
         rlb = floor(rlb);
         rlc = rlc + errtab[dp];
         if (rlc >= 60.0)
           {
            rlc = rlc - 60.0;
            rlb = rlb + 1.0;
           }
         rlc = rlc - errtab[dp];
         if (rlb >= 60.0)
           {
            rlb = rlb - 60.0;
            rla = rla + 1.0;
           }
         while (rla >= 360.0)
           {
            rla = rla - 360.0;
           }
         if (neg)
            rla = - rla;
         sprintf(geo_buffer,"%4.0lf%c %2.0lf' ",rla,gcode,rlb);
         sprintf(wkstrg,strfmt(siz,dp,'F'),rlc);
         len = strlen(wkstrg);
         for (i=0; i<len; i++)
           {
            if (wkstrg[i] == '-')
               wkstrg[i] = ' ';
           }
         strcat(geo_buffer,wkstrg);
         strcat(geo_buffer,tostring('"'));
         break;
        }
      case 'h': ;
      case 'H':      /* ra : hour , minutes , seconds */
        {
         if (dp == 0)
            siz = 2;
         else
            siz = dp + 3;
         if (dp < 0)
            dp = 0;
         hold = hold / 15.0;
         if (hold < 0.0)
           {
            neg = TRUE;
            hold = - hold;
           }
         rla = floor(hold);
         rlb = (hold - rla) * 60.0;
         rlc = (rlb - floor(rlb)) * 60.0;
         rlb = floor(rlb);
         rlc = rlc + errtab[dp];
         if (rlc >= 60.0)
           {
            rlc = rlc - 60.0;
            rlb = rlb + 1.0;
           }
         rlc = rlc - errtab[dp];
         if (rlb >= 60.0)
           {
            rlb = rlb - 60.0;
            rla = rla + 1.0;
           }
         while (rla >= 24.0)
           {
            rla = rla - 24.0;
           }
         if (neg)
            rla = - rla;
         sprintf(geo_buffer,"%3.0lf%c %2.0lfm ",rla,gcode,rlb);
         sprintf(wkstrg,strfmt(siz,dp,'F'),rlc);
         len = strlen(wkstrg);
         for (i=0; i<len; i++)
           {
            if (wkstrg[i] == '-')
               wkstrg[i] = ' ';
           }
         strcat(geo_buffer,wkstrg);
         strcat(geo_buffer,"s");
         break;
        }
      default:
        {
         sprintf(geo_buffer,strfmt(-18,dp,'F'),value);
         strunpad(geo_buffer,' ',17);
        }
     }
   return geo_buffer;
  }



int xstrgeo(char strg[], int pos, int zlen, int dp, double value, char gcode)

  {
   xstrs(strg,pos,zlen,zlen,geostr(value,dp,gcode));
   return strpos;
  }


void showscales(struct scaledata *scales)

  {
   char xstrg[20];
   char ystrg[20];

   strcpy(xstrg,geostr(scales->wx0,2,scales->wxtype));
   strcpy(ystrg,geostr(scales->wy0,2,scales->wytype));
   printf("");
   printf("%14s %14s\n",xstrg,ystrg);
   if (scales->iod == 1)    /* horizontal format */
     {
      putl("                    +-----------+");
      putl("                    |           |");
      putl("                    |           |");
      putl("                    +-----------+");
     }
   else    /* vertical assumed */
     {
      putl("                       +-----+");
      putl("                       |     |");
      putl("                       |     |");
      putl("                       |     |");
      putl("                       |     |");
      putl("                       +-----+");
     }
   strcpy(xstrg,geostr(scales->wx1,2,scales->wxtype));
   strcpy(ystrg,geostr(scales->wy1,2,scales->wytype));
   printf("                        %14s %14s\n\n",xstrg,ystrg);
  }


