/* sidtime.c --  Real-time sidereal clock functions -- looks up or displays UT
                 and calculates sidereal time
                 written as sid_clk by Gary Flispart   11/08/2005
                 separated to library functions        10/17/2007

                 Original functions written by GLF for Commodore 64 from
                 information in an article in Sky & Telescope magazine,
                 later translated to IBM PC-compatible as SIDCLOCK.BAS,
                 converted core functions to C on 11/08/2005

                 06/21/2007 -- add feature: show "clock" position of
                               Polaris in Meade polar axis finder
*/


/* compile using Turbo C 2.0 and HUGE memory model or Dev-C++ */

#include "gflib.h"
#include <math.h>
#include <ctype.h>
#include <time.h>

#if defined(__MINGW32__) || defined(__TURBOC__)   
#include <sys\timeb.h>
#else
/* if Linux, use GNU libc timeval structure instead of timeb */
#endif

#include "sidtime.h"

#define SM_TWO_PI   6.283185307179586476925286766559
#define SM_ONE_PI   3.1415926535897932384626433832795
#define SM_HALF_PI  1.5707963267948966192313216916398
#define SM_VERY_SMALL_NUM  0.0000000000000000001


/* NOTE:  ticks (or jiffies) are defined as 1/60 second increments,
          such that 1 day = 5184000 ticks
*/
#define TICKSPERDAY 5184000L

   
double sm_normalize_angle(double angle)
  {
   while (angle >= SM_TWO_PI)
     {
      angle -= SM_TWO_PI;       
     }

   while (angle < 0.000)
     {
      angle += SM_TWO_PI;       
     }
   
   return angle;  
  }   
   
   
double sm_safe_sqrt(double v)
  {
   if (v < SM_VERY_SMALL_NUM)
      return 0.0;    
   return sqrt(v);
  }   
   
void sm_rotate(double *x, double *y, double theta)
  {
   double orgx, orgy;
   double newx, newy;
   
   orgx = *x;
   orgy = *y;
     
   theta = sm_normalize_angle(theta);  
   
   newx = (*x) * cos(theta) - ((*y) * sin(theta));
   newy = (*x) * sin(theta) + ((*y) * cos(theta));
                                                                \
   *x = newx;
   *y = newy;
  }   
   
double sm_find_angle(double x, double y)
  {
   double theta = SM_ONE_PI;
   double r;
   double factor;
   double cosx, siny;
   
   r = sm_safe_sqrt(x*x+y*y);
   if (r <= SM_VERY_SMALL_NUM)    
     {
      return 0.0;   
     }      

   /* normalize to unit circle */
   factor = 1.0 / r;
   cosx = factor * x;
   siny = factor * y;

   /* make coordinates safe for acos() */
   if (cosx > 1.0)
      {
       cosx = 1.0;     
      }
   if (cosx < -1.0)
      {
       cosx = -1.0;     
      }
    
   if (siny >= 0.0)
     {
      theta = acos(cosx);      
     }   
   else
     {
      theta = acos(-cosx) - SM_ONE_PI;      
     }  
       
   return sm_normalize_angle(theta);    
  }   

   
/* --- sm_calc_precess() Methodology ---------------------------------------

   Given:             RA in hours (0.0 to 24.0)
                      Declination in degrees (-90.0 to 90.0)    
                      Epoch Year for coordinates (usually 2000)
                      Current Date (Year, Mo, Day)
                     
   Calculate:         New RA in hours (0.0 to 24.0)
                      New declination in degrees (-90.0 to 90.0)
                      New coordinates are corrected for effects of 
                      precession of equinox (positive or negative)
                      since coordinate epoch
   Method:
          
      Start with normalized Cartesian coordinate assumptions:

         Sky is a sphere of radius 1.000 with observer at center,
         located at origin of x, y, and z axes.  The x axis defines
         east-west on the ground, the y axis defines north-south on the
         ground, and z axis passes through the observer and the zenith.
         y is positive from origin to north, x is positive from origin 
         to east, and z is positive upwards.
         
         Assume that the sidereal time is equal to RA, that is the object 
         is currently on the meridian.

      Therefore the object lies on a semicircle within the x-z plane
      passing through (0,1,0)=north-on-the-ground, (0,0,1)=zenith, and 
      (0,-1,0)=south-on-the-ground.  
    
      Convert fractions of circles in parameters to Cartesian (x,y,z) coordinates 
      as needed, consistent with normalized assumptions.
      
      Rotate around y axis until RA 12.0 is on meridian.
      
      Assume all of effect of precession of equinox is along this meridian,
      at a rate of 3.33 arcminutes in 10 years. 
       
      Calculate an angle (from difference in date from epoch date) by which 
      to adjust all declinations on this meridian. The entire reference frame
      will be rotated around x axis, carrying the original star's coordinates
      along to a new position. This effect occurs VERY SLOWLY, so the correction 
      need not take into account precision to less than month level in this 
      simplified methodology.
      
      Rotate the reference frame around the y axis by 90 degrees to change the 
      observer to a location at the north pole, carrying the original star to a 
      new position near, but perhaps not ON, the meridian.  The altitude of this 
      new position is the corrected declination and the azimuth MINUS 12.0 hours
      is the corrected RA.  Finally, adjust the RA (only) once more to allow for 
      standardized adjustment of the vernal equinox.
      
   ---------------------------------------------------------------------- */

   
void sm_calc_precess(double *newra, double *newdec, double ra, double dec, 
                             int epochyr, int curyr, int curmo, int curday)
  {
   /* newra and ra in hours, newdec and dec in degrees */                 
   
   double x, y, z;       /* final cartesian coordinates */
   double x1, y1, z1;    /* temporary cartesian coordinates 1 */
   double ha_rad, ra_rad, dec_rad, 
          reverse_ha_rad,
          newra_rad, newdec_rad, az_rad, alt_rad ;   /* convert to radians */ 
   double r;       
   
   double alt, az;
   
   int diffyr, diffmo;
   
   double precess_arcmin, precess_rahours, precess_decangle, precess_raangle;

   
/* the following constants were empirically determined (trial and error) 10/31/2009
   and are hand-optimized for errors < 1/2 minute (RA or Dec) usually much smaller, 
   across a range of years 1982 to 2027 near Polaris.  Errors in declination 
   precession are considerably less than that, and errors in RA precession are 
   reasonably small for other stars in  the northern hemisphere. 
   The methodology uses certain linear assumptions which break down over long 
   periods, but the method is good enough for routine use within the normal 
   lifetime of PC software for the next 25 years. --GLF */

#define SM_ARCMIN_DEC_PRECESS_PER_MO 0.02768

/* if bypassing second-order correction, use 0.0000711 for first-order */

#define SM_FO_HOUR_RA_PRECESS_PER_MO 0.000045    
#define SM_SO_HOUR_RA_PRECESS_PER_MO 0.00000016


   /* 
      C trig functions express angles in radians
      
      for trig functions,  0.0 is at (0,1) and angles run counterclockwise
      for geo angles,      0.0 is North (up) and run clockwise
      
      because of the above, some coordinates are inverted or angles rotated below
   
      for declination      0.0 is on the celestial equator, north is positive
      for RA               0.0 is hour 0.0, all others positive
      for latitude         0.0 is the equator, north is positive 
   */

   /* convert to radians */
   ra_rad = sm_normalize_angle(-((ra/24.0) * SM_TWO_PI));                 
   dec_rad = sm_normalize_angle((dec/360.0) * SM_TWO_PI);                 
   
   /* calculate motion due to precession since epoch date */
   /* assume 4-digit years, e.g. 2000, and months 1-12 */
   
   diffyr = curyr - epochyr;
   if (diffyr < 0)
     {
      diffmo = curmo - 12;         
     }
   else
     {
      diffmo = curmo;    
     }            
     
   diffmo += diffyr*12;  /* diffmo now = total months since epoch (+ or -) */

   precess_arcmin = SM_ARCMIN_DEC_PRECESS_PER_MO * diffmo;
   precess_rahours = (SM_FO_HOUR_RA_PRECESS_PER_MO  + 
                              (SM_SO_HOUR_RA_PRECESS_PER_MO * diffmo)) * diffmo; 

   precess_decangle = sm_normalize_angle((precess_arcmin/21600.0) * SM_TWO_PI);                 
   precess_raangle = sm_normalize_angle((precess_rahours/24.0) * SM_TWO_PI);                 

   /* start hypothetically at equator, sidereal time assumed = RA */

   /* place object in declination -- temporarily place x at 0.0, r = 1.0,
      star at zenith */

   /* x coord held constant for this rotation */
   x = 0.0;
   y = 0.0;
   z = 1.0;
   
   sm_rotate(&z,&y,dec_rad);

   r = z;   /* radius of circle section of sphere, center (0,y,0) 
                with all points at declination */
   /* NOTE:  at this point r WILL ALWAYS be >= 0 */

   /* rotate object in RA such that sidereal time is assumed 0.0, 
      to determine new x */
   /* y coord held constant for this rotation */

   ha_rad = sm_normalize_angle(0.0-ra_rad);                 
   reverse_ha_rad = sm_normalize_angle(-ha_rad);                 
   
   sm_rotate(&z,&x,ha_rad);
           
   /* note that (x,y,z) are now fully defined within temporary frame */        
                                  
                                            
   /* rotate object (as if in latitude, center (x,0,0)) to adjust for 
      precession;  also subtract 90 degrees to rotate observer frame to
      north pole at 0.0 h sidereal time 
      --- x coord held constant for this rotation  */

   sm_rotate(&y,&z,sm_normalize_angle(((90.0/360.0)*SM_TWO_PI)-precess_decangle));

   /* note that (x,y,z) are now fully determined */ 
   
   /* star has been rotated to a new frame with observer at north pole; 
      star may be SLIGHTLY OFF of the meridian now due to precession */
   
   /* the star's altitude should be the new declination, and the azimuth will 
      be the new RA */

   az_rad = sm_find_angle(y,x);
   alt_rad = sm_find_angle(sm_safe_sqrt(x*x + y*y),z);

   /* finally apply an ad-hoc correction to RA to allow for standardized 
      adjustment for the vernal equinox */
   
   newra_rad = sm_normalize_angle(az_rad - precess_raangle - (12.0/24.0)*SM_TWO_PI);
   newdec_rad = sm_normalize_angle(alt_rad);

   /* at this point new declination is correct as angle, but does not properly 
      represent +-90 degrees from the celestial equator, instead the angle 
      is in radians, from 0 to TWOPI.  That is, southern declinations are shown as 
      radian equivalents of 180 to 360 degrees, and northern declinations are
      shown as radian equivalents of 0 to 180 degrees.  Correct that below by
      converting to a semicircular system of +- 90 degrees. 
   */   
   
   if (newdec_rad > SM_ONE_PI)
     {
      /* southern declination */
      if (newdec_rad > (SM_ONE_PI + SM_HALF_PI))  
        {
         newdec_rad = -sm_normalize_angle(SM_TWO_PI - newdec_rad);         
        }       
      else
        {
         newdec_rad = -sm_normalize_angle(newdec_rad - SM_ONE_PI);       
        }
     }
   else
     {
      /* northern declination */   
      if (newdec_rad > SM_HALF_PI)
        {
         newdec_rad = sm_normalize_angle(SM_ONE_PI - newdec_rad);         
        }       
      else
        {
        }
     }  

   ra_rad = sm_normalize_angle(-((ra/24.0) * SM_TWO_PI));                 
   dec_rad = sm_normalize_angle((dec/360.0) * SM_TWO_PI);                 

   *newdec = 360.0 * (newdec_rad / SM_TWO_PI);
   *newra  = 24.0 - (24.0 * (newra_rad / SM_TWO_PI));
  }   
   

/* NOTE:  ticks (or jiffies) are defined as 1/60 second increments,
          such that 1 day = 5184000 ticks
*/

long tick_clock(void)  /* returns current tick (or jiffy) count (24 hr loop) */
  {
   struct tm *timeptr;
   double calc;
#if defined(__MINGW32__) || defined(__TURBOC__)   
   struct timeb tbuf;
#else   
   struct timeval tv;
   time_t tx;
#endif

#if defined(__MINGW32__) || defined(__TURBOC__)   
   ftime(&tbuf);
   timeptr = gmtime(&(tbuf.time));
#else
   gettimeofday(&tv,NULL);
   tx = (time_t)tv.tv_sec;
   timeptr = gmtime(&tx);
#endif

   calc = timeptr->tm_hour * 216000L;
   calc += timeptr->tm_min * 3600L;
   calc += timeptr->tm_sec * 60L;
#if defined(__MINGW32__) || defined(__TURBOC__)   
   calc += tbuf.millitm * 0.06;
#else
   calc += tv.tv_usec * 0.00006;
#endif
   return calc;
  }

/* INNERMAX = 280000 is calibrated to GCC on a 1.8 Mhz computer */
/* INNERMAX = 28000 is a guesstimate for a 200 Mhz computer */
#define INNERMAX 15000L

static long delayval = INNERMAX;

void ms_delay(long ms)   /* delays number of milliseconds -- initially 
                            reasonable, but improve with calibrate_delay() */
  {
   long i,j;
   long toss;
   
   for (i=0; i<ms; i++)
     {
      for (j=0; j<delayval; j++)
        {
         toss = j;
        }
     }  
  }


void calibrate_delay(void)
  {
   /* test actual time for ms_delay() and adjust loop count to compensate */  
   long a;
   long b;
   double secs;

   secs = 0.0;
   a = tick_clock();
   while (secs < 0.0001)
     {
      ms_delay(5000);
      b = tick_clock();
      secs = (b - a)/60.0;
      a = tick_clock();
     } 
   delayval = delayval * (5.0 / secs);
  }


int modaytab[] =
  {
   0,0,31,59,90,120,151,181,212,243,273,304,334
  };


int is_ly(int yr)
  {
   int ly;

   ly = 0;
   if ((yr % 4) == 0)
     {
      ly = 1;
     }
   return ly;
  }


double jd_sid(int yy, int mm, int dd)
  {
   long j;
   int i;

   if ((yy < 1950) || (yy > 2050))
     {
      yy = 1950;
     }

   j = modaytab[mm] + dd - 1;
   if (is_ly(yy))
     {
      if (mm > 2)
        {
         j++;
        }
     }

   j += ((yy - 1949) / 4);
   j += (yy - 1950)*365;

   return (33282.5 + j);
  }


long fracday_to_ticks(double dayfraction)

  {
   long t;

   t = dayfraction * TICKSPERDAY;
   while (t >= TICKSPERDAY)
     {
      t -= TICKSPERDAY;
     }
   while (t < TICKSPERDAY)
     {
      t += TICKSPERDAY;
     }
   return t;
  }


double ticks_to_fracday(long ticks)
  {
   double q;

   q = ticks;
   q = q / TICKSPERDAY;
   return q;
  }

void fracday_to_hms(double dayfraction, int *hd, int *md, int *sd)

  {
   double q;

   q = dayfraction;

   while (q > 1.0)
     {
      q -= 1.0;
     }
   while (q < 0.0)
     {
      q += 1.0;
     }

   q = q * 24.0;
   *hd = q;
   q = (q - *hd) * 60.0;
   *md = q;
   q = (q - *md) * 60.0;
   *sd = q + 0.5;

   if (*sd < 60)
      return;

   *sd = *sd - 60;
   *md = *md + 1;

   if (*md < 60)
      return;

   *md = *md - 60;
   *hd = *hd + 1;

   if (*hd < 24)
      return;

   *hd = *hd - 24;
  }



double hms_to_frac(int lh, int lm, int ls)
  {
   double l;

   l = ls/60.0;
   l = (l+lm)/60.0;
   l = (l+lh)/24.0;
   return l;
  }



double dms_to_frac(int ld, int lm, int ls)
  {
   double l;

   l = ls/60.0;
   l = (l+lm)/60.0;
   l = (l+ld)/360.0;
   return l;
  }



long gmt_secs(void)
  {
   struct tm *timeptr;
   long calc;
#if defined(__MINGW32__) || defined(__TURBOC__)   
   struct timeb tbuf;
#else   
   struct timeval tv;
   time_t tx;
#endif

#if defined(__MINGW32__) || defined(__TURBOC__)   
   ftime(&tbuf);
   timeptr = gmtime(&(tbuf.time));
#else
   gettimeofday(&tv,NULL);
   tx = (time_t)tv.tv_sec;
   timeptr = gmtime(&tx);
#endif

   calc = timeptr->tm_hour * 216000L;
   calc += timeptr->tm_min * 3600L;
   calc += timeptr->tm_sec * 60L;
#if defined(__MINGW32__) || defined(__TURBOC__)   
   calc += tbuf.millitm * 0.06;
#else
   calc += tv.tv_usec * 0.00006;
#endif

   return calc;
  }


long arbitrary_secs(int hi, int mi, int si)
  {
   long calc;


   calc =  hi * 216000L;
   calc += mi * 3600L;
   calc += si * 60L;

   return calc;
  }



double sidereal_fracday(int yr, int mo, int da, long ticks, double longfrac)
  {
   long int_n;
   double n;
   double sv;
   double t;
   double s;

   if ((yr < 1950) ||
       (yr > 2050) ||
       (mo < 1)    ||
       (mo > 12)   ||
       (da < 1)    ||
       (da > 31))
     {
      return 0.0;
     }

   s = ticks_to_fracday(ticks);
   t = (jd_sid(yr,mo,da)-51545.0)/36525.0;
   n = (24110.548 + 8640184.8*t + 0.093104*t*t - 0.0000062*t*t*t) / 86400.0;

   int_n = n;
   sv = n - int_n;
   while (sv < 0.0)
     {
      sv += 1.0;
     }

   s = sv + s + s*.002737845;
   while (s >= 1.0)
     {
      s -= 1.0;
     }

   s = s + longfrac;

   while (s < 0.0)
     {
      s += 1.0;
     }
   while (s >= 1.0)
     {
      s -= 1.0;
     }

   return s;
  }


/*

typedef struct
  {
   int utyr;
   int utmo;
   int utda;
   int uthr;
   int utmin;
   int utsec;
   long utticks;
   int sidhr;
   int sidmin;
   int sidsec;
   long sidticks;
  }
   sidclock;

*/


void ut_sid_time(sidclock *sc, int ld, int lm, int ls)
  {
   struct tm *timeptr;
   long calc;
   double longfrac;
   double s;
#if defined(__MINGW32__) || defined(__TURBOC__)   
   struct timeb tbuf;
#else   
   struct timeval tv;
   time_t tx;
#endif
   double l;

   /* calc longitude as fraction of circumference */
   l = 0.0;
   if ((ld >= -180) &&
       (ld <= 180)  &&
       (lm >= 0)    &&
       (lm <= 59)   &&
       (ls >= 0)    &&
       (ls <= 59))
     {
      l = ls/60.0;
      l = (l+lm)/60.0;
      if (ld < 0)
        {
         l = (ld-l)/360.0;
        }
      else
        {
         l = (l+ld)/360.0;
        }
     }

   /* get UT including millisecond count */
#if defined(__MINGW32__) || defined(__TURBOC__)   
   ftime(&tbuf);
   timeptr = gmtime(&(tbuf.time));
#else
   gettimeofday(&tv,NULL);
   tx = (time_t)tv.tv_sec;
   timeptr = gmtime(&tx);
#endif

   sc->utyr = timeptr->tm_year + 1900;
   sc->utmo = timeptr->tm_mon + 1;
   sc->utda = timeptr->tm_mday;

   sc->uthr = timeptr->tm_hour;
   sc->utmin = timeptr->tm_min;
   sc->utsec = timeptr->tm_sec;


   /* calculate 1/60 second tick count for this UT */
   calc = timeptr->tm_hour * 216000L;
   calc += timeptr->tm_min * 3600L;
   calc += timeptr->tm_sec * 60L;
#if defined(__MINGW32__) || defined(__TURBOC__)   
   calc += tbuf.millitm * 0.06;
#else
   calc += tv.tv_usec * 0.00006;
#endif

   sc->utticks = calc;

   /* calculate associated sidereal time */
   s = sidereal_fracday(sc->utyr,sc->utmo,sc->utda,sc->utticks,l);

   fracday_to_hms(s,&(sc->sidhr),&(sc->sidmin),&(sc->sidsec));
   sc->sidticks = fracday_to_ticks(s);
  }



void sid_time(sidclock *sc, int ld, int lm, int ls,
                            int yr, int mo, int da,
                            int hh, int mm, int ss)
  {
   struct tm *timeptr;
   long calc;
   double longfrac;
   double s;
   double l;

   if ((yr >= 0) &&
       (yr <= 49))
     {
      yr = 2000 + yr;
     }

   if ((yr >= 50) &&
       (yr <= 99))
     {
      yr = 1900 + yr;
     }

   if ((yr < 1950) ||
       (yr > 2050) ||
       (mo < 1)    ||
       (mo > 12)   ||
       (da < 1)    ||
       (da > 31))
     {
      yr = 1950;
      mo = 1;
      da = 1;
     }

   if ((hh < 0)    ||
       (hh > 23)   ||
       (mm < 0)    ||
       (mm > 59)   ||
       (ss < 0)    ||
       (ss > 59))
     {
      hh = 0;
      mm = 0;
      ss = 0;
     }

   /* calc longitude as fraction of circumference */
   l = 0.0;
   if ((ld >= -180) &&
       (ld <= 180)  &&
       (lm >= 0)    &&
       (lm <= 59)   &&
       (ls >= 0)    &&
       (ls <= 59))
     {
      l = ls/60.0;
      l = (l+lm)/60.0;
      if (ld < 0)
        {
         l = (ld-l)/360.0;
        }
      else
        {
         l = (l+ld)/360.0;
        }
     }

   sc->utyr = yr;
   sc->utmo = mo;
   sc->utda = da;

   sc->uthr = hh;
   sc->utmin = mm;
   sc->utsec = ss;


   /* calculate 1/60 second tick count for this UT */
   calc = hh * 216000L;
   calc += mm * 3600L;
   calc += ss * 60L;

   sc->utticks = calc;

   /* calculate associated sidereal time */
   s = sidereal_fracday(sc->utyr,sc->utmo,sc->utda,sc->utticks,l);

   fracday_to_hms(s,&(sc->sidhr),&(sc->sidmin),&(sc->sidsec));
   sc->sidticks = fracday_to_ticks(s);
  }




void s_ymd(char strg[], int yy, int mm, int dd)   /* CAUTION! Make sure strg[] is big enough! */
  {
   sprintf(strg,"\n%04d-%02d-%02d     \n",yy,mm,dd);
  }

void s_hms(char strg[], int hd, int md, int sd)   /* CAUTION! Make sure strg[] is big enough! */
  {
   sprintf(strg,"\n%02d : %02d : %02d   \n",hd,md,sd);
  }

void s_jultime(char strg[], double juliantime)    /* CAUTION! Make sure strg[] is big enough! */
  {
   int hd,md,sd;

   fracday_to_hms(juliantime, &hd, &md, &sd);
   s_hms(strg,hd,md,sd);
  }


double polaris_ra = 0.0;
double polaris_dec = 0.0;


double polaris_finder(sidclock sc)    /* clockface position of polaris in polar finder 
                                         at given siderweal time */
  {
   double stime, ptime, p_ra, p_dec, ftime24;

   if ((sc.sidhr < 0)  || (sc.sidhr > 23))  return 0.0;
   if ((sc.sidmin < 0) || (sc.sidhr > 59))  return 0.0;
   if ((sc.sidsec < 0) || (sc.sidhr > 59))  return 0.0;

   stime = (double)sc.sidsec / 60.0;
   stime = (stime + (double)sc.sidmin) / 60.0;
   stime = (stime + (double)sc.sidhr) /  24.0;

   /* NEED TO PRECESS POLARIS RA to current date! */
   /* right ascension of Polaris 2.5302 - J2000 epoch */
   /* declination of Polaris 89.2642 - J2000 epoch */

   /* convert RA and dec (J2000) of Polaris to current date precessed coordinates */
   sm_calc_precess(&p_ra, &p_dec, 
                   2.5302, 89.2642, 2000, 
                   sc.utyr, sc.utmo, sc.utda);

   polaris_ra = p_ra;
   polaris_dec = p_dec;

   ptime = p_ra / 24.0;   /* right ascension of Polaris - precessed to date */


   ftime24 =  (ptime + 0.5) - stime;
                             /* add 12 hr to invert finder field */





   /* normalize 24-hr clock to 1.000 */
   while (ftime24 >= 1.000)
     {
      ftime24 -= 1.000;
     }

   while (ftime24 < 0.000)
     {
      ftime24 += 1.000;
     }

   return ftime24;
  }

