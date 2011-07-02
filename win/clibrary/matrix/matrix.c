/* matrix.c -- support library for 4 x 4 matrix and 1 x 4 vector operations
               and other functions useful for 3D graphics transforms
               written 2/10/96 by Gary L. Flispart
*/

#include "gflib.h"
/* #include <mem.h> */
#include "obsolete.h"

#include "matrix.h"

#define MINUSNEARZERO -0.0000001
#define PLUSNEARZERO   0.0000001

#define DEG_IN_RADIAN 57.29577951

#define WORKSPACESIZE 16

/*
   Matrix and vector operations use a FIFO workspace to guarantee resolution
   of pointer addresses when stacking together multiple matrix function
   operations -- it is essential that the user not try to put more
   than WORKSPACESIZE matrix or vector functions into a statement line
   before specifically assigning a result to a declared variable, usually
   using the copymat() function.  The limit given above should suffice for
   most programs.
*/


/* ------------------------------------------------------------------------
   The following provide support for FIFO workspace
*/

static matrx matrxwork[WORKSPACESIZE];
static vectr vectrwork[WORKSPACESIZE];

static matrxworkct = 0;
static vectrworkct = 0;

static matrx *newmatrxwork(void)

  {
   matrxworkct++;
   if (matrxworkct >= WORKSPACESIZE)
     {
      matrxworkct = 0;
     }
   return &(matrxwork[matrxworkct]);
  }


static vectr *newvectrwork(void)

  {
   vectrworkct++;
   if (vectrworkct >= WORKSPACESIZE)
     {
      vectrworkct = 0;
     }
   return &(vectrwork[vectrworkct]);
  }


/* the identity matrix is used so often it is declared as a constant here */
static const matrx mat_m0 =     /* identity matrix */
  {
    {
     { 1.0, 0.0, 0.0, 0.0 },
     { 0.0, 1.0, 0.0, 0.0 },
     { 0.0, 0.0, 1.0, 0.0 },
     { 0.0, 0.0, 0.0, 1.0 }
    }
  };

/* the zero vector is also declared as a constant here */
static const vectr vec_v0 =     /* normalized zero vector */
  {  0.0, 0.0, 0.0, 1.0  };


/* ------------------------------------------------------------------------
   The following provide matrix and vector display support for debugging
*/


/* display a 4 x 4 matrix on the console */
void showmat(char strg[], matrx *mat1)

  {
   int i;
   int j;

   printf("%s\n",strg);
   printf("/                                                  \\\n" ) ;
   for (i=0; i<4; i++)
     {
      printf("| ");
      for (j=0; j<4; j++)
        {
         printf("%11.3lf ",mat1->mx[i][j]);
        }
      printf(" |\n");
     }
   printf("\\                                                  /\n\n" ) ;
  }


/* display a 1 x 4 vector on the console */
void showvec(char strg[], vectr *vec1)

  {
   int j;

   printf("%s\n",strg);
   printf("/                                                  \\\n");
   printf("| ");
   for (j=0; j<4; j++)
     {
      printf("%11.3lf ",vec1->vx[j]);
     }
   printf(" |\n");
   printf("\\                                                  /\n\n");
  }


/* display workspace of 4 x 4 matrices on the console */
void showmatrxwork(void)

  {
   int i;
   char strg[20];

   for (i=0; i<WORKSPACESIZE; i++)
     {
      sprintf(strg,"MATRIX WORK %d = ",i);
      showmat(strg,&(matrxwork[i]));
     }
  }

/* display workspace of 1 x 4 vectors on the console */
void showvectrwork(void)

  {
   int i;
   char strg[20];

   for (i=0; i<WORKSPACESIZE; i++)
     {
      sprintf(strg,"VECTOR WORK %d = ",i);
      showvec(strg,&(vectrwork[i]));
     }
  }


/* ------------------------------------------------------------------------
   The following are the matrix and vector operations available to the user
   and which each return a pointer to a vector or matrix created on-the-fly
   in the FIFO workspace.  The workspace allows these operations to be
   combined efficiently within a statement line without keeping track of
   ad-hoc variables.
*/


/* copy one matrix to another */
matrx *copymat(matrx *mat2, matrx *mat1)

  {
   memcpy(mat2,mat1,sizeof(mat_m0));
   return mat2;
  }


/* copy one vector to another */
vectr *copyvec(vectr *vec2, vectr *vec1)

  {
   memcpy(vec2,vec1,sizeof(vec_v0));
   return vec2;
  }


/* sum of two matrices */
matrx *addmat(matrx *mat1, matrx *mat2)

  {
   int i,j,k;
   double temp;
   matrx *mat3;

   mat3 = newmatrxwork();
   for (j=0; j<4; j++)
     {
      for (k=0; k<4; k++)
        {
         mat3->mx[k][j] = mat1->mx[k][j] + mat2->mx[k][j];
        }
     }
   return mat3;
  }


/* sum of two vectors */
vectr *addvec(vectr *vec1, vectr *vec2)

  {
   int k;
   double temp;
   vectr *vec3;

   vec3 = newvectrwork();
   for (k=0; k<4; k++)
     {
      vec3->vx[k] = vec1->vx[k] + vec2->vx[k];
     }
   return vec3;
  }


/* difference of two matrices */
matrx *submat(matrx *mat1, matrx *mat2)

  {
   int i,j,k;
   double temp;
   matrx *mat3;

   mat3 = newmatrxwork();
   for (j=0; j<4; j++)
     {
      for (k=0; k<4; k++)
        {
         mat3->mx[k][j] = mat1->mx[k][j] - mat2->mx[k][j];
        }
     }
   return mat3;
  }


/* difference of two vectors */
vectr *subvec(vectr *vec1, vectr *vec2)

  {
   int k;
   double temp;
   vectr *vec3;

   vec3 = newvectrwork();
   for (k=0; k<4; k++)
     {
      vec3->vx[k] = vec1->vx[k] - vec2->vx[k];
     }
   return vec3;
  }


/* additive negation of a matrix */
matrx *negatemat(matrx *mat1)

  {
   int i,j,k;
   double temp;
   matrx *mat2;

   mat2 = newmatrxwork();
   for (j=0; j<4; j++)
     {
      for (k=0; k<4; k++)
        {
         mat2->mx[k][j] = -(mat1->mx[k][j]);
        }
     }
   return mat2;
  }


/* additive negation of a vector */
vectr *negatevec(vectr *vec1)

  {
   int k;
   double temp;
   vectr *vec2;

   vec2 = newvectrwork();
   for (k=0; k<4; k++)
     {
      vec2->vx[k] = -(vec1->vx[k]);
     }
   return vec2;
  }


/* multiply a matrix by a scalar value */
matrx *multmatby(matrx *mat1, double scalar)

  {
   int i,j,k;
   double temp;
   matrx *mat2;

   mat2 = newmatrxwork();
   for (j=0; j<4; j++)
     {
      for (k=0; k<4; k++)
        {
         mat2->mx[k][j] = scalar * mat1->mx[k][j];
        }
     }
   return mat2;
  }


/* multiply a vector by a scalar value */
vectr *multvecby(vectr *vec1, double scalar)

  {
   int k;
   double temp;
   vectr *vec2;

   vec2 = newvectrwork();
   for (k=0; k<4; k++)
     {
      vec2->vx[k] = vec1->vx[k] * scalar;
     }
   return vec2;
  }


/* multiply a vector by a matrix */
vectr *multvecbymat(vectr *vec1, matrx *mat2)

  {
   int i,j,k;
   double temp;
   vectr *vec3;

   vec3 = newvectrwork();
   for (j=0; j<4; j++)
     {
      temp = 0.0;
      for (k=0; k<4; k++)
        {
         temp = temp + (vec1->vx[k] * mat2->mx[k][j]);
        }
      vec3->vx[j] = temp;
     }
   return vec3;
  }


/* product of two matrices (matrix multiplication) */
matrx *multmat(matrx *mat1, matrx *mat2)

  {
   int i,j,k;
   double temp;
   matrx *mat3;

   mat3 = newmatrxwork();
   for (j=0; j<4; j++)
     {
      for (i=0; i<4; i++)
        {
         temp = 0.0;
         for (k=0; k<4; k++)
           {
            temp = temp + (mat1->mx[i][k] * mat2->mx[k][j]);
           }
         mat3->mx[i][j] = temp;
        }
     }
   return mat3;
  }


/* provide the matrix for overall scaling transformation */
matrx *mat_scale(double scale)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[0][0] = scale;
   mat->mx[1][1] = scale;
   mat->mx[2][2] = scale;
   return mat;
  }


/* provide the matrix for x, y, z scaling transformation */
matrx *mat_scalexyz(double x, double y, double z)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[0][0] = x;
   mat->mx[1][1] = y;
   mat->mx[2][2] = z;
   return mat;
  }


/* provide the matrix for x axis rotation transformation */
matrx *mat_rotx(double radians)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[1][1] = cos(radians);
   mat->mx[2][2] = mat->mx[1][1];
   mat->mx[1][2] = sin(radians);
   mat->mx[2][1] = -(mat->mx[1][2]);
   return mat;
  }


/* provide the matrix for y axis rotation transformation */
matrx *mat_roty(double radians)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[0][0] = cos(radians);
   mat->mx[2][2] = mat->mx[0][0];
   mat->mx[2][0] = sin(radians);
   mat->mx[0][2] = -(mat->mx[2][0]);
   return mat;
  }



/* provide the matrix for z axis rotation transformation */
matrx *mat_rotz(double radians)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[0][0] = cos(radians);
   mat->mx[1][1] = mat->mx[0][0];
   mat->mx[0][1] = sin(radians);
   mat->mx[1][0] = -(mat->mx[0][1]);
   return mat;
  }


/* provide a matrix from literal values */
matrx *matrix(double a, double b, double c, double d,
              double e, double f, double g, double h,
              double i, double j, double k, double l,
              double m, double n, double o, double p)

  {
   matrx *mat1;

   mat1 = newmatrxwork();

   mat1->mx[0][0] = a;
   mat1->mx[0][1] = b;
   mat1->mx[0][2] = c;
   mat1->mx[0][3] = d;
   mat1->mx[1][0] = e;
   mat1->mx[1][1] = f;
   mat1->mx[1][2] = g;
   mat1->mx[1][3] = h;
   mat1->mx[2][0] = i;
   mat1->mx[2][1] = j;
   mat1->mx[2][2] = k;
   mat1->mx[2][3] = l;
   mat1->mx[3][0] = m;
   mat1->mx[3][1] = n;
   mat1->mx[3][2] = o;
   mat1->mx[3][3] = p;
  }


/* provide a vector from literal values */
vectr *vector(double a, double b, double c, double d)

  {
   vectr *vec1;

   vec1 = newvectrwork();
   vec1->vx[0] = a;
   vec1->vx[1] = b;
   vec1->vx[2] = c;
   vec1->vx[3] = d;
  }


/* provide the identity matrix */
matrx *mat_identity(void)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   return mat;
  }


/* provide the zero vector */
vectr *vec_zero(void)

  {
   vectr *vec;

   vec = newvectrwork();
   memcpy(vec,&vec_v0,sizeof(vec_v0));
   return vec;
  }


/* provide the matrix for translation by x, y, z */
matrx *mat_transxyz(double x, double y, double z)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[3][0] = x;
   mat->mx[3][1] = y;
   mat->mx[3][2] = z;
   return mat;
  }


/* provide the matrix for reflection of x across yz plane */
matrx *mat_reflectx(void)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[0][0] = -1.0;
   return mat;
  }


/* provide the matrix for reflection of y across xz plane */
matrx *mat_reflecty(void)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[1][1] = -1.0;
   return mat;
  }


/* provide the matrix for reflection of z across xy plane */
matrx *mat_reflectz(void)

  {
   matrx *mat;

   mat = newmatrxwork();
   memcpy(mat,&mat_m0,sizeof(mat_m0));
   mat->mx[2][2] = -1.0;
   return mat;
  }



/* ------------------------------------------------------------------------
   The following functions return values rather than pointers.
*/


/* determinant of a matrix */
double determinant(matrx *mat1)

  {
   double a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
   double pk,gl,ho,kh,ol,pg,ej,fm,in,mj,ne,fi;

   a = mat1->mx[0][0];
   b = mat1->mx[0][1];
   c = mat1->mx[0][2];
   d = mat1->mx[0][3];
   e = mat1->mx[1][0];
   f = mat1->mx[1][1];
   g = mat1->mx[1][2];
   h = mat1->mx[1][3];
   i = mat1->mx[2][0];
   j = mat1->mx[2][1];
   k = mat1->mx[2][2];
   l = mat1->mx[2][3];
   m = mat1->mx[3][0];
   n = mat1->mx[3][1];
   o = mat1->mx[3][2];
   p = mat1->mx[3][3];

   pk = p * k;
   gl = g * l;
   ho = h * o;
   kh = k * h;
   ol = o * l;
   pg = p * g;
   ej = e * j;
   fm = f * m;
   in = i * n;
   mj = m * j;
   ne = n * e;
   fi = f * i;

   return (    a * ( f*pk + n*gl + j*ho - n*kh - f*ol - j*pg )
             - b * ( e*pk + m*gl + i*ho - m*kh - e*ol - i*pg )
             + c * ( p*ej + l*fm + h*in - h*mj - l*ne - p*fi )
             - d * ( o*ej + k*fm + g*in - g*mj - k*ne - o*fi ) );
  }



/* inverse of a matrix -- returns 0 (FALSE) and sets matrix mat2 to
   identity matrix if unable to invert (inverse undefined), else
   returns 1 (TRUE) and sets matrix mat2 to inverse matrix           */

int invertmat(matrx *mat2, matrx *mat1)

  {
   double val;
   int i,j,k,l;
   double s,t;
   int ok;

   matrx *mat3;

   mat3 = newmatrxwork();

   memcpy(mat3,mat1,sizeof(mat_m0));
   memcpy(mat2,&mat_m0,sizeof(mat_m0));

   for (j=0; j<4; j++)
     {
      ok = 0;
      for (i=j; i<4; i++)
        {
         if ((mat3->mx[j][i] <= MINUSNEARZERO) ||
             (mat3->mx[j][i] >= PLUSNEARZERO)     )
           {
            ok = 1;
            break;
           }
        }
      if (!ok)
         return 0;    /* singular matrix -- uninvertable */
      for (k=0; k<4; k++)
        {
         s = mat3->mx[j][k];
         mat3->mx[j][k] = mat3->mx[i][k];
         mat3->mx[i][k] = s;

         s = mat2->mx[j][k];
         mat2->mx[j][k] = mat2->mx[i][k];
         mat2->mx[i][k] = s;
        }
      t = 1.0;
      t = t / (mat3->mx[j][j]);
      for (k=0; k<4; k++)
        {
         mat3->mx[j][k] = t * mat3->mx[j][k];
         mat2->mx[j][k] = t * mat2->mx[j][k];
        }
      for (l=0; l<4; l++)
        {
         if (l != j)
           {
            t = -(mat3->mx[l][j]);
            for (k=0; k<4; k++)
              {
               mat3->mx[l][k] = mat3->mx[l][k] + (t * mat3->mx[j][k]);
               mat2->mx[l][k] = mat2->mx[l][k] + (t * mat2->mx[j][k]);
              }
           }
        }
     }
   return 1;
  }


/* converts radians to degrees */
double todegrees(double radians)

  {
   return radians * DEG_IN_RADIAN;
  }


/* converts degrees to radians */
double toradians(double degrees)

  {
   return degrees / DEG_IN_RADIAN;
  }


/* The following are examples of vector and matrix constant assignments:

   static vectr vec1 =
     {   2.0,  1.0,  3.0,  1.0 } ;

   static matrx mat1 =
     {
       {
        {  1.0,  4.0,  2.0,  0.0  },
        {  3.0,  0.0, -1.0,  0.0  },
        {  1.0,  5.0,  2.0,  0.0  },
        {  0.0,  0.0,  0.0,  0.0  }
       }
     };

 */

