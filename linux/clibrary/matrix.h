/* matrix.h -- header file for
               support library for 4 x 4 matrix and 1 x 4 vector operations
               and other functions useful for 3D graphics transforms
               written 2/10/96 by Gary L. Flispart
*/

/*
   Matrix and vector operations use a FIFO workspace to guarantee resolution
   of pointer addresses when stacking together multiple matrix function
   operations -- it is essential that the user not try to put more
   than the limit (16) of matrix or vector functions into a statement line
   before specifically assigning a result to a declared variable, usually
   using the copymat() function.  The limit should suffice for
   most programs.
*/

#ifndef MATRIX_H__
#define MATRIX_H__

#include <math.h>
#include <string.h>


typedef struct
  {
   double mx[4][4];
  }
   matrx;

typedef struct
  {
   double vx[4];
  }
   vectr;


/* ------------------------------------------------------------------------
   The following provide matrix and vector display support for debugging
*/


/* display a 4 x 4 matrix on the console */
void showmat(char strg[], matrx *mat1);

/* display a 1 x 4 vector on the console */
void showvec(char strg[], vectr *vec1);

/* display workspace of 4 x 4 matrices on the console */
void showmatrxwork(void);

/* display workspace of 1 x 4 vectors on the console */
void showvectrwork(void);


/* ------------------------------------------------------------------------
   The following are the matrix and vector operations available to the user
   and which each return a pointer to a vector or matrix created on-the-fly
   in the FIFO workspace.  The workspace allows these operations to be
   combined efficiently within a statement line without keeping track of
   ad-hoc variables.
*/


/* copy one matrix to another */
matrx *copymat(matrx *mat2, matrx *mat1);

/* copy one vector to another */
vectr *copyvec(vectr *vec2, vectr *vec1);

/* sum of two matrices */
matrx *addmat(matrx *mat1, matrx *mat2);

/* sum of two vectors */
vectr *addvec(vectr *vec1, vectr *vec2);

/* difference of two matrices */
matrx *submat(matrx *mat1, matrx *mat2);

/* difference of two vectors */
vectr *subvec(vectr *vec1, vectr *vec2);

/* additive negation of a matrix */
matrx *negatemat(matrx *mat1);

/* additive negation of a vector */
vectr *negatevec(vectr *vec1);

/* multiply a matrix by a scalar value */
matrx *multmatby(matrx *mat1, double scalar);

/* multiply a vector by a scalar value */
vectr *multvecby(vectr *vec1, double scalar);

/* multiply a vector by a matrix */
vectr *multvecbymat(vectr *vec1, matrx *mat2);

/* product of two matrices (matrix multiplication) */
matrx *multmat(matrx *mat1, matrx *mat2);

/* provide the matrix for overall scaling transformation */
matrx *mat_scale(double scale);

/* provide the matrix for x, y, z scaling transformation */
matrx *mat_scalexyz(double x, double y, double z);

/* provide the matrix for x axis rotation transformation */
matrx *mat_rotx(double radians);

/* provide the matrix for y axis rotation transformation */
matrx *mat_roty(double radians);

/* provide the matrix for z axis rotation transformation */
matrx *mat_rotz(double radians);

/* provide a matrix from literal values */
matrx *matrix(double a, double b, double c, double d,
              double e, double f, double g, double h,
              double i, double j, double k, double l,
              double m, double n, double o, double p);

/* provide a vector from literal values */
vectr *vector(double a, double b, double c, double d);

/* provide the identity matrix */
matrx *mat_identity(void);

/* provide the zero vector */
vectr *vec_zero(void);

/* provide the matrix for translation by x, y, z */
matrx *mat_transxyz(double x, double y, double z);

/* provide the matrix for reflection of x across yz plane */
matrx *mat_reflectx(void);

/* provide the matrix for reflection of y across xz plane */
matrx *mat_reflecty(void);

/* provide the matrix for reflection of z across xy plane */
matrx *mat_reflectz(void);

/* ------------------------------------------------------------------------
   The following functions return values rather than pointers.
*/

/* determinant of a matrix */
double determinant(matrx *mat1);

/* inverse of a matrix -- returns 0 (FALSE) and sets matrix mat2 to
      identity matrix if unable to invert (inverse undefined), else
      returns 1 (TRUE) and sets matrix mat2 to inverse matrix           */
int invertmat(matrx *mat2, matrx *mat1);

/* converts radians to degrees */
double todegrees(double radians);

/* converts degrees to radians */
double toradians(double degrees);


/* the following are examples of matrix and vector constant assignments:

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

#endif
