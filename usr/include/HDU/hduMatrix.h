/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduMatrix.h

Description: 

  Provides basic functionality of a 4x4 transform matrix.

*******************************************************************************/

#ifndef hduMatrix_H_
#define hduMatrix_H_

#include <HDU/hduGenericMatrix.h>
#include <HDU/hduVector.h>

#include <memory.h>
#include <float.h>
#include <ostream>

class hduQuaternion;

class hduMatrix
{    
public:
    /* Default constructor. */
    hduMatrix()
    {
        makeIdentity();
    }
    
    /* Constructor from array of 16 values stored by row. */
    explicit hduMatrix(const double * const mat);
    
    /* Construct from 16 values.  Values construct the following matrix:
       a1  a2  a3  a4
       a5  a6  a7  a8
       a9 a10 a11 a12
       a13 a14 a15 a16 */
    hduMatrix(double a1, double a2, double a3, double a4,
              double a5, double a6, double a7, double a8,
              double a9, double a10, double a11, double a12,
              double a13, double a14, double a15, double a16)
    {
        double a[4][4] = 
        { 
            {a1,a2,a3,a4},
            {a5,a6,a7,a8},
            {a9,a10,a11,a12},
            {a13,a14,a15,a16}
        };
        set (a);
    }

    /* Constructor from 4x4 array of values. */
    explicit hduMatrix(const double a[4][4])
    {
        set(a);
    }
    
    /* Compare matrices (returns true if all elements
       of one matrix are within epsilon of the other). */
    bool compare(const hduMatrix & rhs, double epsilon = DBL_EPSILON) const;
    
    /* Comparison operator (==). */
    bool operator ==(const hduMatrix & rhs) const
    {
        return compare(rhs);
    }
    
    /* Comparison operator (!=). */
    bool operator !=(const hduMatrix & rhs) const
    {
        return !compare(rhs);
    }
    
    /* Get value at location (i,j). */
    double get(const int i, const int j) const
    {
        return m_elements[i][j];
    }
    
    /* Set value at location (i,j). */
    void set(const int i, const int j, const double value)
    {
        m_elements[i][j] = value;
    }
    
    /* operator() get element (i,j). */
    double &operator()(const int i, const int j)
    {
        return m_elements[i][j];
    }
    const double & operator()(const int i, const int j) const
    {
        return m_elements[i][j];
    }
    
    /* operator[][] returns element (i,j). */
    double * operator [](const int i) 
    {
        return &m_elements[i][0];
    }    
    const double * operator[](int i) const 
    {
        return &m_elements[i][0];
    }
    
    /* Convert to array. */
    operator const double *() const { return &m_elements[0][0]; }
    operator double *() { return &m_elements[0][0]; }   

    /* Multiplies this matrix by given matrix on left. */
    hduMatrix &multLeft(const hduMatrix &m);

    /* Multiplies this matrix by given matrix on right. */
    hduMatrix &multRight(const hduMatrix &m);

    /* Matrix Multiplication (operator *). */
    hduMatrix operator *(const hduMatrix &rhs) const;
    
    /* Matrix Multiplication (operator *=). */
    hduMatrix & operator *=(const hduMatrix &rhs);
    
    /* Multiply matrix by column vector and returns vector result
       Assumes homogenous coordinate is 1. */
    void multMatrixVec(const hduVector3Dd &src, hduVector3Dd &dst) const;
    
    /* Multiply row vector by matrix and returns vector result
       Assumes homogenous coordinate is 1. */
    void multVecMatrix(const hduVector3Dd &src, hduVector3Dd &dst) const;
    
    /* Multiply matrix by column direction vector and return vector result.
       This version ignores the homogeneous component, so the translation
       part of the matrix is ignored. Note: Use this routine with the inverse
       transpose of the matrix for transforming normals. */
    void multDirMatrix(const hduVector3Dd &src, hduVector3Dd &dst) const;

    /* Multiply column direction vector by matrix and return vector result.
       This version ignores the homogeneous component, so the translation
       part of the matrix is ignored. Note: Use this routine with the inverse
       transpose of the matrix for transforming normals. */
    void multMatrixDir(const hduVector3Dd &src, hduVector3Dd &dst) const;

    /* Sets to identity matrix. */
    void makeIdentity()
    {
        hduGenericMatrix::makeIdentity4x4(*this);
    }
    
    /* Tests if matrix is an identity matrix. */
    bool isIdentity() const;
    
    /* Get transpose of matrix. */
    hduMatrix getTranspose() const;
    
    /* Set value of matrix to its transpose. */
    void transpose();    
    
    /* Get inverse of matrix.  'success' is set to TRUE if the matrix was 
       inverted (i.e. has an inverse), and FALSE if the matrix inverse failed 
       (e.g. the matrix was singular).  The return value is indeterminate if 
       success is FALSE.  Calling getInverse without the 'success' variable will
       also yield indeterminate results if no inverse for the matrix exists. */
    hduMatrix getInverse() const;
    hduMatrix getInverse(bool & success) const;
    
    /* Sets value of matrix to inverse.  
       Returns TRUE if the matrix was successfully inverted, FALSE if there
       is no inverse to the matrix (e.g. the matrix is singular).
       The matrix is not modified (i.e. is unchanged as a result of
       calling the function) if the function returns FALSE. */
    bool invert();

    /* Decompose matrix and recover the upper 3x3 rotation matrix */
    void getRotationMatrix(hduMatrix &rotMat) const;
    void getRotationMatrix(double R[3][3]) const;
    void getRotation(hduQuaternion &quat) const;

    /* Set 4x4 array. */
    void set(const double a[4][4])
    {
        copy4x4(m_elements, a);
    }
    
    /* Set 4x4 array. */
    void set(const hduMatrix & m)
    {
        set(m.m_elements);
    }
    
    /* Get 4x4 array. */
    void get(double a[4][4]) const
    {
        copy4x4(a, m_elements);
    }
    
    /* Copies values from copyFromMat to copyToMat. */
    inline static void copy4x4(double dst[4][4],
                               const double src[4][4])
    {
        memcpy(dst, src, sizeof(double) * 16);
    }
    
    /* Returns a matrix with the given translation.
       Translations are set along the last row of the matrix.  
       The generated translation matrix is:
       1 0 0 0
       0 1 0 0
       0 0 1 0
       x y z 1 */
    static hduMatrix createTranslation(const double x, 
                                       const double y, 
                                       const double z);
    static hduMatrix createTranslation(const hduVector3Dd &t);
    
    /* Returns a matrix with the given scale.
       The generated scale matrix is:
       x 0 0 0
       0 y 0 0
       0 0 z 0
       0 0 0 1 */
    static hduMatrix createScale(const double x, 
                                 const double y, 
                                 const double z);
    static hduMatrix createScale(const hduVector3Dd &s);
    

    /* Returns a matrix with the given rotation.
       The matrix is created with a rotation of <radians> 
       radians around the specified <v> vector.
       <v> is required to have non-zero magnitude.  Other than that,
       the magnitude of <v> does not affect the rotation (i.e.
       a normalized version of <v> is used to determine the
       rotation). */
    static hduMatrix createRotation(const hduVector3Dd &v,
                                    const double radians);
    
    /* Returns a matrix with the given rotation.
       The matrix is created with a rotation of <radians> 
       radians around a vector with components {x,y,z}.
       The vector {x,y,z} is required to have non-zero magnitude.  
       Other than that, the magnitude of the vector does not affect the 
       rotation (i.e. a normalized version of {x,y,z} is used to 
       determine the rotation). */
    static hduMatrix createRotation(const double x, 
                                    const double y, 
                                    const double z, 
                                    const double radians);
    
    /* Returns a hduMatrix with a rotation of <radians> radians 
       around the X axis. */
    static hduMatrix createRotationAroundX(const double radians) 
    {
        return createRotation(hduVector3Dd(1,0,0),radians);
    }
    
    /* Returns a hduMatrix with a rotation of <angle> radians around 
       the Y axis. */
    static hduMatrix createRotationAroundY(const double radians) 
    {
        return createRotation(hduVector3Dd(0,1,0),radians);
    }
    
    /* Returns a hduMatrix with a rotation of <angle> radians around 
       the Z axis. */
    static hduMatrix createRotationAroundZ(const double radians) 
    {
        return createRotation(hduVector3Dd(0,0,1),radians);
    }

    /* Composes a transform matrix consisting of a rotation due to a
       quaternion representation and a translation */
    static hduMatrix createRotationTranslation(const hduQuaternion &quat, 
                                               const hduVector3Dd &t);
    
private:
    double m_elements[4][4];
    
};

/******************************************************************************
 Printing
******************************************************************************/
inline std::ostream& operator<<(std::ostream& os, const hduMatrix &mat)
{
	return hduGenericMatrix::output<hduMatrix, 4, 4, std::ostream>(os, mat);
}

/******************************************************************************
 Vector-matricx multiplication
******************************************************************************/
inline hduVector3Dd operator *(const hduVector3Dd &v,
                               const hduMatrix &mat)
{
    hduVector3Dd res;
    mat.multVecMatrix(v, res);
    return res;
}

/******************************************************************************
 Matrix and vector multiplication
******************************************************************************/
inline hduVector3Dd operator *(const hduMatrix &mat,
                               const hduVector3Dd &v)
{
    hduVector3Dd res;
    mat.multMatrixVec(v, res);
    return res;
}

/******************************************************************************
 Constructor from array of 16 values stored by row.
******************************************************************************/
inline hduMatrix::hduMatrix(const double * const mat)
{
    for (int i=0;i < 4 ;++i )
    {
        for (int j=0;j < 4 ;++j )
        {
            set(i,j,mat[4*i+j]);
        }
    }    
} 

/******************************************************************************
 Compare matrices (returns true if all elements of one matrix are within 
 epsilon of the other).
******************************************************************************/
inline bool hduMatrix::compare(const hduMatrix& rhs,
                               double epsilon) const
{
    return hduGenericMatrix::compare<hduMatrix, hduMatrix, 4, 4, double>(
        *this, rhs, epsilon);
}

/******************************************************************************
 Multiplies this matrix by given matrix on left.
******************************************************************************/
inline hduMatrix &hduMatrix::multLeft(const hduMatrix &m)
{
    hduMatrix res;
    hduGenericMatrix::mulMatrixMatrix4x4(res, m, *this);
    set(res);
    return *this;
}

/******************************************************************************
 Multiplies this matrix by given matrix on right.
******************************************************************************/
inline hduMatrix &hduMatrix::multRight(const hduMatrix &m)
{
    hduMatrix res;
    hduGenericMatrix::mulMatrixMatrix4x4(res, *this, m);
    set(res);
    return *this;    
}

/******************************************************************************
 Matrix Multiplication (operator *).
******************************************************************************/
inline hduMatrix hduMatrix::operator *(const hduMatrix &rhs) const
{
    hduMatrix res;
    hduGenericMatrix::mulMatrixMatrix4x4(res, *this, rhs);
    return res;
}

/******************************************************************************
 Matrix Multiplication (operator *=).
******************************************************************************/
inline hduMatrix &hduMatrix::operator *=(const hduMatrix &rhs)
{
    hduMatrix res;
    hduGenericMatrix::mulMatrixMatrix4x4(res, *this, rhs);
    set(res);
    return *this;
}

/******************************************************************************
 Multiply matrix by column vector and returns vector result
 Assumes homogenous coordinate is 1.
******************************************************************************/
inline void hduMatrix::multMatrixVec(const hduVector3Dd &src, 
                                     hduVector3Dd &dst) const
{
    HDdouble resw[4];
    HDdouble srcw[4] = { src[0], src[1], src[2], 1.0 };
    hduGenericMatrix::mulMatrix4x4Point4(resw, *this, srcw);
    dst[0] = resw[0] / resw[3];
    dst[1] = resw[1] / resw[3];
    dst[2] = resw[2] / resw[3];
}

/******************************************************************************
 Multiply row vector by matrix and returns vector result
 Assumes homogenous coordinate is 1.
******************************************************************************/
inline void hduMatrix::multVecMatrix(const hduVector3Dd &src, 
                                     hduVector3Dd &dst) const
{
    HDdouble resw[4];
    HDdouble srcw[4] = { src[0], src[1], src[2], 1.0 };
    hduGenericMatrix::mulPoint4Matrix4x4(resw, srcw, *this);
    dst[0] = resw[0] / resw[3];
    dst[1] = resw[1] / resw[3];
    dst[2] = resw[2] / resw[3];
}

/******************************************************************************
 Multiply matrix by column direction vector and return vector result.
 This version ignores the homogeneous component, so the translation
 part of the matrix is ignored. Note: Use this routine with the inverse
 transpose of the matrix for transforming normals.
******************************************************************************/
inline void hduMatrix::multDirMatrix(const hduVector3Dd &src, 
                                     hduVector3Dd &dst) const
{
    hduVector3Dd res;
    hduGenericMatrix::mulVector3Matrix4x4(res, src, *this); 
    dst = res;
}

/******************************************************************************
 Multiply column direction vector by matrix and return vector result.
 This version ignores the homogeneous component, so the translation
 part of the matrix is ignored. Note: Use this routine with the inverse
 transpose of the matrix for transforming normals.
******************************************************************************/
inline void hduMatrix::multMatrixDir(const hduVector3Dd &src, 
                                     hduVector3Dd &dst) const
{
    hduVector3Dd res;
    hduGenericMatrix::mulMatrix4x4Vector3(res, *this, src); 
    dst = res;
}

/******************************************************************************
 Get transpose of matrix.
******************************************************************************/
inline hduMatrix hduMatrix::getTranspose() const
{
    hduMatrix m;
    hduGenericMatrix::transpose4x4(m, *this);
    return m;
}

/******************************************************************************
 Set value of matrix to its transpose.
******************************************************************************/
inline void hduMatrix::transpose()
{
    hduMatrix m;
    hduGenericMatrix::transpose4x4(m, *this);
    set(m);
}

/******************************************************************************
 Sets value of matrix to its inverse.  
 Returns TRUE if the matrix was successfully inverted, FALSE if there is no 
 inverse to the matrix (e.g. the matrix is singular).  The matrix is not 
 modified (i.e. is unchanged as a result of calling the function) if the 
 function returns FALSE.
******************************************************************************/
inline bool hduMatrix::invert() 
{   
    bool success;
    hduMatrix m = getInverse(success);
    if (success)
        set(m);
    return success;
} 

/******************************************************************************
 Get inverse of matrix.  The result is indeterminate if no inverse exists for 
 the matrix (e.g. the matrix is singular).
******************************************************************************/
inline hduMatrix hduMatrix::getInverse() const
{
    bool success;
    return getInverse(success);
}

/******************************************************************************
 Tests if matrix is an identity matrix.
******************************************************************************/
inline bool hduMatrix::isIdentity() const
{
    hduMatrix i;
    //i.makeIdentity(); // redundant
    return compare(i,.0001);
}

#endif  /* hduMatrix_H_ */

/*****************************************************************************/
