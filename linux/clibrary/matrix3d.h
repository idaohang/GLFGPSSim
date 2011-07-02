/* matrix3d.h -- support for 3d matrix transformations */

/* 3D transformation Matrix definitions

     |M11 M12 M13 M14|
     |M21 M22 M23 M24|
     |M31 M32 M33 M34|
     |M41 M42 M43 M44|


Assuming an original solid model with world coordinates centered at (0,0,0):

Identity matrix:    Scale by S      Rotate model about     Translate model
(no change to       in direction    z axis by angle @:     coordinates by
 original)          of z axis:                             x, y, z:

     |1 0 0 0|      |1 0 0 0|       | cos@  sin @  0  0|   |1 0 0 0|
     |0 1 0 0|      |0 1 0 0|       |-sin@  cos @  0  0|   |0 1 0 0|
     |0 0 1 0|      |0 0 S 0|       |  0     0     1  0|   |0 0 1 0|
     |0 0 0 1|      |0 0 0 1|       |  0     0     0  1|   |x y z 1|

*/

#ifndef MATRIX3D_H__
#define MATRIX3D_H__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef BOOL
#define BOOL short int
#endif

#ifndef FLOAT
#define FLOAT float
#endif

#ifndef LONG
#define LONG long int
#endif

#ifndef RECT
typedef struct
  {
   LONG left;
   LONG top;
   LONG right;
   LONG bottom;
  }
   RECT;
#endif

#ifndef POINT
typedef struct
  {
   LONG x;
   LONG y;
  }
   POINT;
#endif

typedef struct
  {
   FLOAT vx[4];
  }
   VECTOR3D;


typedef struct
  {
   FLOAT mx[4][4];
  }
   MATRIX3D;

typedef struct
  {
   FLOAT x;
   FLOAT y;
   FLOAT z;
  }
   POINT3D;

typedef struct
  {
   FLOAT yaw;
   FLOAT pitch;
   FLOAT roll;
  }
   ANGLE3D;



MATRIX3D *Mat3D_Identity(MATRIX3D *matrix);
MATRIX3D *Mat3D_Translate(MATRIX3D *matrix, double x, double y, double z);
MATRIX3D *Mat3D_Scale(MATRIX3D *matrix, double x, double y, double z);
MATRIX3D *Mat3D_ReflectX(MATRIX3D *matrix);
MATRIX3D *Mat3D_ReflectY(MATRIX3D *matrix);
MATRIX3D *Mat3D_ReflectZ(MATRIX3D *matrix);
MATRIX3D *Mat3D_Zoom(MATRIX3D *matrix, double zoom);

#define DEGREES_TO_RADIANS 0.017453293
#define RADIANS_TO_DEGREES 57.29577951

#define MINUSNEARZERO -0.0000001
#define PLUSNEARZERO   0.0000001


/* rotate about the X axis */
MATRIX3D *Mat3D_RotateX(MATRIX3D *matrix, double degrees);

/* rotate about the Y axis */
MATRIX3D *Mat3D_RotateY(MATRIX3D *matrix, double degrees);

/* rotate about the Z axis */
MATRIX3D *Mat3D_RotateZ(MATRIX3D *matrix, double degrees);

/* copy one matrix to another */
MATRIX3D *Mat3D_Copy(MATRIX3D *mat2, MATRIX3D *mat1);

/* copy one vector to another */
VECTOR3D *Mat3D_CopyVector(VECTOR3D *vec2, VECTOR3D *vec1);

/* sum of two matrices */
MATRIX3D *Mat3D_Add(MATRIX3D *mat3, MATRIX3D *mat1, MATRIX3D *mat2);

/* sum of two vectors */
VECTOR3D *Mat3D_AddVector(VECTOR3D *vec3, VECTOR3D *vec1, VECTOR3D *vec2);

/* difference of two matrices */
MATRIX3D *Mat3D_Sub(MATRIX3D *mat3, MATRIX3D *mat1, MATRIX3D *mat2);

/* difference of two vectors */
VECTOR3D *Mat3D_SubVector(VECTOR3D *vec3, VECTOR3D *vec1, VECTOR3D *vec2);

/* additive negation of a matrix */
MATRIX3D *Mat3D_Neg(MATRIX3D *mat2, MATRIX3D *mat1);

/* additive negation of a vector */
VECTOR3D *Mat3D_NegVector(VECTOR3D *vec2, VECTOR3D *vec1);

/* multiply a matrix by a scalar value */
MATRIX3D *Mat3D_MultVal(MATRIX3D *mat2, MATRIX3D *mat1, double scalar);

/* multiply a vector by a scalar value */
VECTOR3D *Mat3D_MultVectorVal(VECTOR3D *vec2, VECTOR3D *vec1, double scalar);

/* multiply a vector by a matrix */
VECTOR3D *Mat3D_MultVectorMat(VECTOR3D *vec3, VECTOR3D *vec1, MATRIX3D *mat2);

/* product of two matrices (matrix multiplication) */
MATRIX3D *Mat3D_Mult(MATRIX3D *mat3, MATRIX3D *mat1, MATRIX3D *mat2);

/* provide a matrix from given values */
MATRIX3D *Mat3D_Load(MATRIX3D *mat1,
                        double a, double b, double c, double d,
                        double e, double f, double g, double h,
                        double i, double j, double k, double l,
                        double m, double n, double o, double p);

/* provide a vector from given values */
VECTOR3D *Mat3D_LoadVector(VECTOR3D *vec1, double a, double b, double c, double d);

/* provide the zero vector */
VECTOR3D *Mat3D_VectorZero(VECTOR3D *vec);


/* ------------------------------------------------------------------------
   The following functions return values rather than pointers.
*/


/* determinant of a matrix */
double Mat3D_Determinant(MATRIX3D *mat1);

/* inverse of a matrix -- returns FALSE and sets matrix mat2 to
   identity matrix if unable to invert (inverse undefined), else
   returns TRUE and sets matrix mat2 to inverse matrix           */

BOOL Mat3D_Invert(MATRIX3D *mat2, MATRIX3D *mat1);


/* ------------------------------------------------------------------------
   The following provide matrix and vector display support for debugging
*/


/* display a 1 x 4 vector on the console */
void Mat3D_ShowVector(char strg[], VECTOR3D *vec1);

/* display a 4 x 4 matrix on the console */
void Mat3D_Show(char strg[], MATRIX3D *mat1);



#endif
