/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hduGenericMatrix.h

Description: 

  Generic (templatized) matrix utility functions.
 
  These routines support any nxm matrix class as long as the class supports 
  operator[] for element access (i.e. as long as m[i][j] returns the element 
  in row i, column j).  Note this will work for 2D arrays (e.g. double[4][4]) 
  in addition to classes with operator[].

  The matrix dimensions must be known at compile time so that they may be 
  passed in as template parameters.  For example, to multiply a 4x6 matrix 
  times a 6x8 matrix:
 
  >  My4x6MatrixClass m1;
  >  My6x8MatrixClass m2;
  >  My4x8MatrixClass result;
  >  hduGenericMatrix::mulMatrixMatrix<My4x6MatrixClass,
  >                                    My6x8MatrixClass,
  >                                    My4x8MatrixClass,
  >                                    4,6,8>(result, m1, m2);
 
  There are generally separate routines for 4x4 matrices and arbirtrary mxn 
  matrices where the 4x4 matrix routines are optimized for 4x4 matrices and 
  do not require the dimensions as template arguments.  For example to multiply 
  two 4x4 matrices:

  >  My4x4MatrixClass m1, m2, result;
  >  hduGenericMatrix::mulMatrixMatrix4x4(result, m1, m2);

  The set 4x4 matrix routines are used in the class
  hduBasicTransformMatrix and it is generally simpler to use that class
  rather than these routines directly, especially since
  hduBasicTransformMatrix defines operators (*, ==, =, etc...) that are
  more convenient than calling the routines below.  However if you have
  your own matrix, point, or vector classes that you wish to use in
  conjunction with GHOST SDK matrix classes, the routines below may be
  used to help.  For example, you can use the mulMatrixMatrix4x4 routine
  to define an operator* that can multiply your 4x4 matrix with a
  hduTransformMatrix:
 
  >  hduTransformMatrix operator*(
  >             const hduTransformMatrix &m1,
  >             const MyMatrixClass &m2)
  >  {
  >       hduTransformMatrix res;
  >       hduGenericMatrix::mulMatrixMatrix4x4(res, m1, m2);
  >  }

  Similarly, you can use the routine mulPoint3Matrix4x4 to add an operator* 
  that can multiply a hduPoint by your own matrix or vice versa.

*******************************************************************************/

#ifndef hduGenericMatrix_H_
#define hduGenericMatrix_H_
#include <iostream>

namespace hduGenericMatrix
{

/******************************************************************************
 Multiply point3 * 4x4 matrix (res = p * m).  
 Point should have 3 coordinates (x,y,z) and is treated as row vector.
 Routine uses 1 for fourth coordinate.
 Use for standard point * transform matrix. 
******************************************************************************/
template <class PT1, class PT2, class MAT>
inline void mulPoint3Matrix4x4(PT1 & res,    
                               const PT2 & p, 
                               const MAT & m)
{
    res[0] = p[0] * m[0][0] + p[1] * m[1][0] + 
        p[2] * m[2][0] + m[3][0];
    res[1] = p[0] * m[0][1] + p[1] * m[1][1] + 
        p[2] * m[2][1] + m[3][1];
    res[2] = p[0] * m[0][2] + p[1] * m[1][2] + 
        p[2] * m[2][2] + m[3][2];
}

/******************************************************************************
 Multiplies point4 * 4x4 matrix (res = p * m).
 Point should have 4 coordinates (x,y,z,w) and is treated as row vector. 
******************************************************************************/
template <class PT1, class PT2, class MAT>
inline void mulPoint4Matrix4x4(PT1 & res, 
                               const PT2 & p, 
                               const MAT & m)
{
    res[0] = p[0] * m[0][0] + p[1] * m[1][0] + 
        p[2] * m[2][0] + p[3] * m[3][0];
    res[1] = p[0] * m[0][1] + p[1] * m[1][1] + 
        p[2] * m[2][1] + p[3] * m[3][1];
    res[2] = p[0] * m[0][2] + p[1] * m[1][2] + 
        p[2] * m[2][2] + p[3] * m[3][2];
    res[3] = p[0] * m[0][3] + p[1] * m[1][3] + 
        p[2] * m[2][3] + p[3] * m[3][3];
}

/******************************************************************************
 Multiplies n dimensional point times mxn matrix (res = p * m). 
 p should have DIMM coordinates and is treated as row vector.
 res should have DIMN coordinates.
 matrix m should be DIMM x DIMN.
******************************************************************************/
template <class PT1, class PT2, class MAT, int DIMM, int DIMN>
inline void mulPointMatrix(PT1 & res, 
                           const PT2 & p, 
                           const MAT & m)
{
    for (int i = 0; i < DIMN; ++i)
    {
        res[i] = 0;
        for (int j = 0; j < DIMM; ++j)
        {
            res[i] += p[j] * m[j][i];
        }
    }
}

/******************************************************************************
 Multiplies 4x4 matrix * point3 (res = m * p).  
 Point should have 3 coordinates (x,y,z) and is treated as column vector.
 Routine uses 1 for fourth coordinate. 
******************************************************************************/
template <class PT1, class PT2, class MAT>
inline void mulMatrix4x4Point3(PT1 & res,    
                               const MAT & m,
                               const PT2 & p) 
{
    res[0] = p[0] * m[0][0] + p[1] * m[0][1] + 
        p[2] * m[0][2] + m[0][3];
    res[1] = p[0] * m[1][0] + p[1] * m[1][1] + 
        p[2] * m[1][2] + m[1][3];
    res[2] = p[0] * m[2][0] + p[1] * m[2][1] + 
        p[2] * m[2][2] + m[2][3];
}

/******************************************************************************
 Multiplies 4x4 matrix * point4 (res = m * p).
 Point should have 4 coordinates (x,y,z,w) and is treated as column vector. 
******************************************************************************/
template <class PT1, class PT2, class MAT>
inline void mulMatrix4x4Point4(PT1 & res, 
                               const MAT & m, 
                               const PT2 & p)
{
    res[0] = p[0] * m[0][0] + p[1] * m[0][1] + 
        p[2] * m[0][2] + p[3] * m[0][3];
    res[1] = p[0] * m[1][0] + p[1] * m[1][1] + 
        p[2] * m[1][2] + p[3] * m[1][3];
    res[2] = p[0] * m[2][0] + p[1] * m[2][1] + 
        p[2] * m[2][2] + p[3] * m[2][3];
    res[3] = p[0] * m[3][0] + p[1] * m[3][1] + 
        p[2] * m[3][2] + p[3] * m[3][3];
}

/******************************************************************************
 Multiplies mxn matrix * n dimensional point (res = m * p). 
 p should have DIMN coordinates and is treated as column vector.
 res should have DIMM coordinates.
 matrix m should be DIMM x DIMN. 
******************************************************************************/
template <class PT1, class PT2, class MAT, int DIMM, int DIMN>
inline void mulMatrixPoint(PT1 & res, 
                           const MAT & m, 
                           const PT2 & p)
{
    for (int i = 0; i < DIMM; ++i)
    {
        res[i] = 0;
        for (int j = 0; j < DIMN; ++j)
        {
            res[i] += p[j] * m[i][j];
        }
    }
}

/******************************************************************************
 Multiplies vector3 * 4x4 matrix (res = v * m).  
 Vector should have 3 coordinates (x,y,z) and is treatd as row vector.
 Routine uses 0 for fourth coordinate.
 Use for standard vector * transform matrix.
 ( Note that this transforms vector as an offset and does not apply for 
   transforming normal vectors. )
******************************************************************************/
template <class VEC1, class VEC2, class MAT>
inline void mulVector3Matrix4x4(VEC1 & res,    
                                const VEC2 & v, 
                                const MAT & m)
{
    res[0] = v[0] * m[0][0] + v[1] * m[1][0] + 
        v[2] * m[2][0];
    res[1] = v[0] * m[0][1] + v[1] * m[1][1] + 
        v[2] * m[2][1];
    res[2] = v[0] * m[0][2] + v[1] * m[1][2] + 
        v[2] * m[2][2];
}

/******************************************************************************
 Multiplies 4x4 matrix * vector3 (res = m * v).  
 Vector should have 3 coordinates (x,y,z) and is treated as column vector.
 Routine uses 0 for fourth coordinate.
 ( Note that this transforms vector as an offset and does not apply for 
   transforming normal vectors. )
******************************************************************************/
template <class VEC1, class VEC2, class MAT>
inline void mulMatrix4x4Vector3(VEC1 & res,    
                                const MAT & m,
                                const VEC2 & v) 
{
    res[0] = v[0] * m[0][0] + v[1] * m[0][1] + 
        v[2] * m[0][2];
    res[1] = v[0] * m[1][0] + v[1] * m[1][1] + 
        v[2] * m[1][2];
    res[2] = v[0] * m[2][0] + v[1] * m[2][1] + 
        v[2] * m[2][2];
}


/******************************************************************************
 Multiplies mxn matrix times nxl matrix (res = m1 * m2).  
 m1 should be DIMMxDIMN matrix.
 m2 should be DIMNxDIML matrix.
 res should be DIMMxDIML matrix.
******************************************************************************/
template <class MAT1, class MAT2, class MAT3, int DIMM, int DIMN, int DIML>
inline void mulMatrixMatrix(MAT1 & res, 
                            const MAT2 & m1, 
                            const MAT3 & m2)
{

    for (int i = 0; i < DIMM; ++i)
    {
        for (int j = 0; j < DIML; ++j)
        {
            double sum = 0;
            for (int k = 0; k < DIMN; ++k)
            {
                sum += m1[i][k] * m2[k][j];
            }
            res[i][j] = sum;
        }
    }
}

/******************************************************************************
 Multiplies two 4x4 matrices (res = m1 * m2). 
******************************************************************************/
template <class MAT1, class MAT2, class MAT3>
inline void mulMatrixMatrix4x4(MAT1 & res, 
                               const MAT2 & m1, 
                               const MAT3 & m2)
{

    res[0][0] =  
        m1[0][0]*m2[0][0] +
        m1[0][1]*m2[1][0] +
        m1[0][2]*m2[2][0] +
        m1[0][3]*m2[3][0];
        
    res[0][1] = 
        m1[0][0]*m2[0][1] +
        m1[0][1]*m2[1][1] +
        m1[0][2]*m2[2][1] +
        m1[0][3]*m2[3][1];
        
    res[0][2] =
        m1[0][0]*m2[0][2] +
        m1[0][1]*m2[1][2] +
        m1[0][2]*m2[2][2] +
        m1[0][3]*m2[3][2];
        
    res[0][3] =
        m1[0][0]*m2[0][3] +
        m1[0][1]*m2[1][3] +
        m1[0][2]*m2[2][3] +
        m1[0][3]*m2[3][3];
        
    res[1][0] =
        m1[1][0]*m2[0][0] +
        m1[1][1]*m2[1][0] +
        m1[1][2]*m2[2][0] +
        m1[1][3]*m2[3][0];
        
    res[1][1] = 
        m1[1][0]*m2[0][1] +
        m1[1][1]*m2[1][1] +
        m1[1][2]*m2[2][1] +
        m1[1][3]*m2[3][1];
        
    res[1][2] = 
        m1[1][0]*m2[0][2] +
        m1[1][1]*m2[1][2] +
        m1[1][2]*m2[2][2] +
        m1[1][3]*m2[3][2];
        
    res[1][3] = 
        m1[1][0]*m2[0][3] +
        m1[1][1]*m2[1][3] +
        m1[1][2]*m2[2][3] +
        m1[1][3]*m2[3][3];
        
    res[2][0] = 
        m1[2][0]*m2[0][0] +
        m1[2][1]*m2[1][0] +
        m1[2][2]*m2[2][0] +
        m1[2][3]*m2[3][0];
        
    res[2][1] = 
        m1[2][0]*m2[0][1] +
        m1[2][1]*m2[1][1] +
        m1[2][2]*m2[2][1] +
        m1[2][3]*m2[3][1];
        
    res[2][2] = 
        m1[2][0]*m2[0][2] +
        m1[2][1]*m2[1][2] +
        m1[2][2]*m2[2][2] +
        m1[2][3]*m2[3][2];
        
    res[2][3] = 
        m1[2][0]*m2[0][3] +
        m1[2][1]*m2[1][3] +
        m1[2][2]*m2[2][3] +
        m1[2][3]*m2[3][3];
        
    res[3][0] = 
        m1[3][0]*m2[0][0] +
        m1[3][1]*m2[1][0] +
        m1[3][2]*m2[2][0] +
        m1[3][3]*m2[3][0];
        
    res[3][1] = 
        m1[3][0]*m2[0][1] +
        m1[3][1]*m2[1][1] +
        m1[3][2]*m2[2][1] +
        m1[3][3]*m2[3][1];
        
    res[3][2] = 
        m1[3][0]*m2[0][2] +
        m1[3][1]*m2[1][2] +
        m1[3][2]*m2[2][2] +
        m1[3][3]*m2[3][2];

    res[3][3] =  
        m1[3][0]*m2[0][3] +
        m1[3][1]*m2[1][3] +
        m1[3][2]*m2[2][3] +
        m1[3][3]*m2[3][3];
    
}

/******************************************************************************
 Transposes mxn matrix.
 Result is nxm matrix. 
******************************************************************************/
template <class MAT1, class MAT2, int DIMM, int DIMN>
inline void transpose(MAT1 & mtrans, const MAT2 & m)
{
    for (int i = 0; i < DIMM; ++i)
    {
        for (int j = 0; j < DIMN; ++j)
        {
            mtrans[i][j] = m[j][i];
        }
    }
}

/******************************************************************************
 Transposes 4x4 matrix.
 Result is 4x4 matrix. 
******************************************************************************/
template <class MAT1, class MAT2>
inline void transpose4x4(MAT1 & mtrans, const MAT2 & m)
{
    mtrans[0][0] = m[0][0];
    mtrans[0][1] = m[1][0];
    mtrans[0][2] = m[2][0];
    mtrans[0][3] = m[3][0];
    mtrans[1][0] = m[0][1];
    mtrans[1][1] = m[1][1];
    mtrans[1][2] = m[2][1];
    mtrans[1][3] = m[3][1];
    mtrans[2][0] = m[0][2];
    mtrans[2][1] = m[1][2];
    mtrans[2][2] = m[2][2];
    mtrans[2][3] = m[3][2];
    mtrans[3][0] = m[0][3];
    mtrans[3][1] = m[1][3];
    mtrans[3][2] = m[2][3];
    mtrans[3][3] = m[3][3];
}

/******************************************************************************
 Copies mxn matrix src into dst.
 ( Note memcpy is generally faster, however this routine allows copying from
   one type of matrix to another as long as both support the [] operator. )
******************************************************************************/
template <class MAT1, class MAT2, int DIMM, int DIMN>
inline void copy(MAT1 & dst, const MAT2 & src)
{
    for (int i = 0; i < DIMM; ++i)
    {
        for (int j = 0; j < DIMN; ++j)
        {
            dst[i][j] = src[i][j];
        }
    }
}

/******************************************************************************
 Copies 4x4 matrix src into dst.
 ( Note memcpy is generally faster, however this routine allows copying from
   one type of matrix to another as long as both support the [] operator. )
******************************************************************************/
template <class MAT1, class MAT2>
inline void copy4x4(MAT1 & dst, const MAT2 & src)
{
    dst[0][0] = src[0][0]; 
    dst[0][1] = src[0][1];
    dst[0][2] = src[0][2];
    dst[0][3] = src[0][3];
    dst[1][0] = src[1][0]; 
    dst[1][1] = src[1][1];
    dst[1][2] = src[1][2];
    dst[1][3] = src[1][3];
    dst[2][0] = src[2][0]; 
    dst[2][1] = src[2][1];
    dst[2][2] = src[2][2];
    dst[2][3] = src[2][3];
    dst[3][0] = src[3][0]; 
    dst[3][1] = src[3][1];
    dst[3][2] = src[3][2];
    dst[3][3] = src[3][3];
}

/******************************************************************************
 Compares two mxn matrixes element by element.
 If any two corresponding elements differ by more than epsilon, returns false. 
******************************************************************************/
template <class MAT1, class MAT2, 
    int DIMM, int DIMN, 
    class EPSTYPE>
inline bool compare(const MAT1 & m1, 
                    const MAT2 & m2, 
                    EPSTYPE epsilon)
{
    for (int i = 0 ; i < DIMM; ++i)
    {
        for (int j = 0; j < DIMM; ++j)
        {
            if ((m1[i][j] - m2[i][j] > epsilon) ||
                (m1[i][j] - m2[i][j] < -epsilon))
                return false;
        }
    }
    return true;
}

/******************************************************************************
 Writes mxn matrix to ostream. 
******************************************************************************/
template <class MAT, 
          int DIMM, 
          int DIMN, 
          class STREAM>
inline STREAM& output(STREAM& s, const MAT & mat)
{
    for (int i = 0; i < DIMM; ++i)
    {
        for (int j = 0; j < DIMN; ++j)
        {
            s << mat[i][j] << ' ';
        }
        s << std::endl;
    }
    return s;
}

/******************************************************************************
 Sets nxn matrix to be identity. 
******************************************************************************/
template <class MAT, int DIM>
inline void makeIdentity(MAT & mat)
{
    for (int i = 0; i < DIM; ++i)
    {
        for (int j = 0; j < DIM; ++j)
        {
            mat[i][j] = (i == j) ? 1 : 0;
        }
    }
}

/******************************************************************************
 Sets 4x4 matrix to be identity.
******************************************************************************/
template <class MAT>
inline void makeIdentity4x4(MAT & mat)
{
    mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1;
    mat[0][1] = mat[0][2] = mat[0][3] = 
    mat[1][0] = mat[1][2] = mat[1][3] = 
    mat[2][0] = mat[2][1] = mat[2][3] =
    mat[3][0] = mat[3][1] = mat[3][2] = 0;
}

} /* namespace hduGenericMatrix */


#endif /* hduGenericMatrix_H_ */

/*****************************************************************************/
