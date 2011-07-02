/* matrix3d.c -- support for 3d matrix transformations */

#include "obsolete.h"

#include "matrix3d.h"

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


Camera position is specified in world coordinates (same as model).
Camera aim is specified in yaw, pitch, and roll angles,
such that with 0,0,0 (yaw,pitch,roll), yaw rotates about x axis,
pitch rotates about y axis, and roll rotates about z axis.
When viewing planar bitmaps, the bitmap is loaded by default assuming
that it lies in the XY plane without rotation (Xworld = Xbmp,
Yworld = Ybmp); camera views it from a distance of 1000 pixels directly
over (bmp_width/2, bmp_height/2) with x axis viewed horizontal, increasing
right, and y axis viewed vertical, increasing upwards (screen Y
coordiunates of bitmap are negated).

Eye coordinates are normalized such that eye has a fixed viewpoint and
orientation facing the screen viewport from a distance of 1000 pixels, centered
over the point (viewport_width/2, viewport_height/2).

Therefore, transforming BMP coordinates through an identity matrix will
yield a view on the screen indistinguishable from a straight 2D mapping
from bitmap to screen at 1:1 scale.

NOTE:

The transformation matrix is EYE-CENTRIC!  That is, the display algorithm
scans all displayable pixels (screen coordinates) with the matrix populated
with values which transform the screen coordintes to the SOURCE coordinates
of the original bitmap closest to the idealized center of the viewing
direction.

*/





MATRIX3D *Mat3D_Identity(MATRIX3D *matrix)
  {
   matrix->mx[0][0]  = 1.0;
   matrix->mx[0][1]  = 0.0;
   matrix->mx[0][2]  = 0.0;
   matrix->mx[0][3]  = 0.0;
   matrix->mx[1][0]  = 0.0;
   matrix->mx[1][1]  = 1.0;
   matrix->mx[1][2]  = 0.0;
   matrix->mx[1][3]  = 0.0;
   matrix->mx[2][0]  = 0.0;
   matrix->mx[2][1]  = 0.0;
   matrix->mx[2][2]  = 1.0;
   matrix->mx[2][3]  = 0.0;
   matrix->mx[3][0]  = 0.0;
   matrix->mx[3][1]  = 0.0;
   matrix->mx[3][2]  = 0.0;
   matrix->mx[3][3]  = 1.0;

   return matrix;
  }


MATRIX3D *Mat3D_Translate(MATRIX3D *matrix, double x, double y, double z)
  {
   Mat3D_Identity(matrix);

   matrix->mx[3][0]  = x;
   matrix->mx[3][1]  = y;
   matrix->mx[3][2]  = z;

   return matrix;
  }


MATRIX3D *Mat3D_Scale(MATRIX3D *matrix, double x, double y, double z)
  {
   Mat3D_Identity(matrix);

   matrix->mx[0][0] = x;
   matrix->mx[1][1] = y;
   matrix->mx[2][2] = z;

   return matrix;
  }


MATRIX3D *Mat3D_ReflectX(MATRIX3D *matrix)
  {
   return Mat3D_Scale(matrix,-1.0,0.0,0.0);
  }


MATRIX3D *Mat3D_ReflectY(MATRIX3D *matrix)
  {
   return Mat3D_Scale(matrix,0.0,-1.0,0.0);
  }


MATRIX3D *Mat3D_ReflectZ(MATRIX3D *matrix)
  {
   return Mat3D_Scale(matrix,0.0,0.0,-1.0);
  }


MATRIX3D *Mat3D_Zoom(MATRIX3D *matrix, double zoom)
  {
   return Mat3D_Scale(matrix,zoom,zoom,zoom);
  }


#define DEGREES_TO_RADIANS 0.017453293
#define RADIANS_TO_DEGREES 57.29577951
#define MINUSNEARZERO -0.0000001
#define PLUSNEARZERO   0.0000001



/* rotate about the X axis */

MATRIX3D *Mat3D_RotateX(MATRIX3D *matrix, double degrees)
  {
   double radians;
   double a,b;

   radians = degrees * DEGREES_TO_RADIANS;
   a = cos(radians);
   b = sin(radians);

   Mat3D_Identity(matrix);

   matrix->mx[1][1] = a;
   matrix->mx[1][2] = b;
   matrix->mx[2][1] = -b;
   matrix->mx[2][2] = a;

   return matrix;
  }


/* rotate about the Y axis */

MATRIX3D *Mat3D_RotateY(MATRIX3D *matrix, double degrees)
  {
   double radians;
   double a,b;

   radians = degrees * DEGREES_TO_RADIANS;
   a = cos(radians);
   b = sin(radians);

   Mat3D_Identity(matrix);

   matrix->mx[0][0] = a;
   matrix->mx[0][2] = -b;
   matrix->mx[2][0] = b;
   matrix->mx[2][2] = a;

   return matrix;
  }


/* rotate about the Z axis */

MATRIX3D *Mat3D_RotateZ(MATRIX3D *matrix, double degrees)
  {
   double radians;
   double a,b;

   radians = degrees * DEGREES_TO_RADIANS;
   a = cos(radians);
   b = sin(radians);

   Mat3D_Identity(matrix);

   matrix->mx[0][0] = a;
   matrix->mx[0][1] = b;
   matrix->mx[1][0] = -b;
   matrix->mx[1][1] = a;

   return matrix;
  }




#ifndef MoveMemory
#define MoveMemory(a,b,c)   memcpy(a,b,c)
#endif

/* copy one matrix to another */
MATRIX3D *Mat3D_Copy(MATRIX3D *mat2, MATRIX3D *mat1)

  {
   MoveMemory(mat2,mat1,sizeof(MATRIX3D));
   return mat2;
  }


/* copy one vector to another */
VECTOR3D *Mat3D_CopyVector(VECTOR3D *vec2, VECTOR3D *vec1)

  {
   MoveMemory(vec2,vec1,sizeof(VECTOR3D));
   return vec2;
  }


/* sum of two matrices */
MATRIX3D *Mat3D_Add(MATRIX3D *mat3, MATRIX3D *mat1, MATRIX3D *mat2)

  {
   int i,j,k;
   FLOAT temp;

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
VECTOR3D *Mat3D_AddVector(VECTOR3D *vec3, VECTOR3D *vec1, VECTOR3D *vec2)

  {
   int k;
   FLOAT temp;

   for (k=0; k<4; k++)
     {
      vec3->vx[k] = vec1->vx[k] + vec2->vx[k];
     }
   return vec3;
  }


/* difference of two matrices */
MATRIX3D *Mat3D_Sub(MATRIX3D *mat3, MATRIX3D *mat1, MATRIX3D *mat2)

  {
   int i,j,k;
   FLOAT temp;

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
VECTOR3D *Mat3D_SubVector(VECTOR3D *vec3, VECTOR3D *vec1, VECTOR3D *vec2)

  {
   int k;
   FLOAT temp;

   for (k=0; k<4; k++)
     {
      vec3->vx[k] = vec1->vx[k] - vec2->vx[k];
     }
   return vec3;
  }


/* additive negation of a matrix */
MATRIX3D *Mat3D_Neg(MATRIX3D *mat2, MATRIX3D *mat1)

  {
   int i,j,k;
   FLOAT temp;

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
VECTOR3D *Mat3D_NegVector(VECTOR3D *vec2, VECTOR3D *vec1)

  {
   int k;
   FLOAT temp;

   for (k=0; k<4; k++)
     {
      vec2->vx[k] = -(vec1->vx[k]);
     }
   return vec2;
  }


/* multiply a matrix by a scalar value */
MATRIX3D *Mat3D_MultVal(MATRIX3D *mat2, MATRIX3D *mat1, double scalar)

  {
   int i,j,k;
   FLOAT temp;

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
VECTOR3D *Mat3D_MultVectorVal(VECTOR3D *vec2, VECTOR3D *vec1, double scalar)

  {
   int k;
   FLOAT temp;

   for (k=0; k<4; k++)
     {
      vec2->vx[k] = vec1->vx[k] * scalar;
     }
   return vec2;
  }


/* multiply a vector by a matrix */
VECTOR3D *Mat3D_MultVectorMat(VECTOR3D *vec3, VECTOR3D *vec1, MATRIX3D *mat2)

  {
   int i,j,k;
   FLOAT temp;

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
MATRIX3D *Mat3D_Mult(MATRIX3D *mat3, MATRIX3D *mat1, MATRIX3D *mat2)

  {
   int i,j,k;
   FLOAT temp;

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



/* provide a matrix from given values */
MATRIX3D *Mat3D_Load(MATRIX3D *mat1,
                        double a, double b, double c, double d,
                        double e, double f, double g, double h,
                        double i, double j, double k, double l,
                        double m, double n, double o, double p)

  {
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

   return mat1;
  }


/* provide a vector from given values */
VECTOR3D *Mat3D_LoadVector(VECTOR3D *vec1, double a, double b, double c, double d)

  {
   vec1->vx[0] = a;
   vec1->vx[1] = b;
   vec1->vx[2] = c;
   vec1->vx[3] = d;

   return vec1;
  }


/* provide the zero vector */
VECTOR3D *Mat3D_VectorZero(VECTOR3D *vec)

  {
   Mat3D_LoadVector(vec,0.0,0.0,0.0,1.0);
   return vec;
  }



/* ------------------------------------------------------------------------
   The following functions return values rather than pointers.
*/


/* determinant of a matrix */
double Mat3D_Determinant(MATRIX3D *mat1)

  {
   FLOAT a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
   FLOAT pk,gl,ho,kh,ol,pg,ej,fm,in,mj,ne,fi;

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



/* inverse of a matrix -- returns FALSE and sets matrix mat2 to
   identity matrix if unable to invert (inverse undefined), else
   returns TRUE and sets matrix mat2 to inverse matrix           */

BOOL Mat3D_Invert(MATRIX3D *mat2, MATRIX3D *mat1)

  {
   FLOAT val;
   int i,j,k,l;
   FLOAT s,t;
   BOOL ok;

   ok = FALSE;

   for (j=0; j<4; j++)
     {
      ok = 0;
      for (i=j; i<4; i++)
        {
         if ((mat1->mx[j][i] <= MINUSNEARZERO) ||
             (mat1->mx[j][i] >= PLUSNEARZERO)     )
           {
            ok = TRUE;
            break;
           }
        }

      if (!ok)
        {
         Mat3D_Identity(mat2);
         return FALSE;    /* singular matrix -- uninvertable */
        }

      Mat3D_Copy(mat2,mat1);

      for (k=0; k<4; k++)
        {
         s = mat2->mx[j][k];
         mat2->mx[j][k] = mat2->mx[i][k];
         mat2->mx[i][k] = s;
        }


      t = 1.0;
      t = t / (mat2->mx[j][j]);
      for (k=0; k<4; k++)
        {
         mat2->mx[j][k] = t * mat2->mx[j][k];
        }
      for (l=0; l<4; l++)
        {
         if (l != j)
           {
            t = -(mat2->mx[l][j]);
            for (k=0; k<4; k++)
              {
               mat2->mx[l][k] = mat2->mx[l][k] + (t * mat2->mx[j][k]);
              }
           }
        }
     }
   return TRUE;
  }


/* The following are examples of vector and matrix constant assignments:

   static VECTOR3D vec1 =
     {   2.0,  1.0,  3.0,  1.0 } ;

   static MATRIX3D mat1 =
     {
       {
        {  1.0,  4.0,  2.0,  0.0  },
        {  3.0,  0.0, -1.0,  0.0  },
        {  1.0,  5.0,  2.0,  0.0  },
        {  0.0,  0.0,  0.0,  0.0  }
       }
     };

 */


/* ------------------------------------------------------------------------
   The following provide matrix and vector display support for debugging
*/


/* display a 1 x 4 vector on the console */
void Mat3D_ShowVector(char strg[], VECTOR3D *vec1)

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


/* display a 4 x 4 matrix on the console */
void Mat3D_Show(char strg[], MATRIX3D *mat1)

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


