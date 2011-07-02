/* +++Date last modified: 05-Jul-1997 */

/*
 *  VIOIMAGE.H; VidMgr routines for saving and restoring text images.
 *              Release 1.2.
 *
 *  This module written in May 1996 by Andrew Clarke and released to the
 *  public domain.
 */

#ifndef __VIOIMAGE_H__
#define __VIOIMAGE_H__

#include <string.h>
#include <math.h>


 /* do not define for Linux yet 12/14/2008 GLF */
#if defined(__TURBOC__) || defined(__MINGW32__)

typedef struct
{
    char *image;
    char width;
    char height;
}
VIOIMAGE;

void vioImageDefaults(VIOIMAGE * v);
int vioImageInit(VIOIMAGE * v, char width, char height);
int vioImageTerm(VIOIMAGE * v);
int vioImageSave(VIOIMAGE * v, char x, char y);
int vioImageRestore(VIOIMAGE * v, char x, char y);

#endif  /* only DOS and Windows */

#endif