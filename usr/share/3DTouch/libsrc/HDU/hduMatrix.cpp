/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hduMatrix.cpp

Description: 

  Provides basic functionality of a 4x4 transform matrix.

*******************************************************************************/

#include "hduAfx.h"

#include "hduDecompose.h"
#include <HDU/hduMatrix.h>
#include <HDU/hduQuaternion.h>
#include <assert.h>

namespace 
{

/* This is a standard routine for solving linear systems AX=B.  Since we are 
   using this to calculate inverses, B is set to the identity matrix.  The 
   function returns false if A is singular.  Otherwise, the return is the X 
   hduMatrix that represents the inverse of A.  Note this function is optimized 
   to solve inverses of 4x4 matrices. */

bool solveSystemViaLUD4x4(
    const double *A, /* input - NxN matrix in eqn AX=B to solve for X. */
    hduMatrix &X)  /* output - NxM solution matrix. */

{ 
    const int N = 4;
    const int M = 4;
    bool    status = true;
    int     i,j,k,l;
    double  *alpha;
    const double  *a_ptr, *b_ptr; /* pointers to rows of matrices. */
    double *alpha_ptr, *alpha_ptr2;
    
    double  sum;
    double  max;
    int     p;
    double temp[N * (N + M)];
    double  swaptemp, maxtemp;
    double B[16] = 
    {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };  
    
    /* First, form alpha as [A | B]. */
    const int NpM = N + M;
    alpha = temp;
    if (! alpha)
    {
        status = false;
        goto wrapup;
    }
    
    for (i = 0; i < N; i++)
    {
        a_ptr = &A[i*N];
        alpha_ptr = &alpha[i*NpM];
        for (j = 0; j < N; j++)
        {
            alpha_ptr[j] = a_ptr[j];
        }
        b_ptr = &B[i*M];
        for (j = 0; j < M; j++)
        {
            alpha_ptr[N+j] = b_ptr[j];
        }
    }
    
    /* Now, decompose A into LU (actually a row permutation of this). 
       Decomposition occurs in place within alpha. */
    for (i = 0; i < N; i++)
    {
        /* Find maximum for pivot element of column. */
        max = -1.0;
        p = i;
        for (j = i; j < N; j++)
        {
            sum = 0.0;
            for (k = 0; k <= i-1; k++)
            {
                sum += alpha[j*NpM+k] * alpha[k*NpM+i];
            }               
            maxtemp = fabs(alpha[j*NpM+i] - sum);
            if (maxtemp > max)
            {
                max = maxtemp;
                p = j;
            }
        } /* end for j */
        /* Now, swap rows i and p (unless they are the same) of alpha. */
        if (p != i)
        {
            alpha_ptr = &alpha[i*NpM];
            alpha_ptr2 = &alpha[p*NpM];
            for (j = 0; j < NpM; j++)
            {
                swaptemp = alpha_ptr[j];
                alpha_ptr[j] = alpha_ptr2[j];
                alpha_ptr2[j] = swaptemp;
            } /* end for j */
        }
        /* Now, compute new diagonal element. */
        alpha_ptr = &alpha[i*NpM];
        sum = 0.0;
        for (k = 0; k <= i-1; k++)
        {
            sum += alpha_ptr[k] * alpha[k*NpM+i];
        }
        alpha_ptr[i] = alpha_ptr[i] - sum;
        
        /* Now, compute new columns (L part of matrix) and rows (U part of 
           matrix). */
        for (j = i+1; j < N; j++)
        {
            alpha_ptr2 = &alpha[j*NpM];
            /* first, the columns */
            sum = 0.0;
            for (k = 0; k <= i-1; k++)
            {
                sum += alpha_ptr2[k] * alpha[k*NpM+i];
            }
            if (alpha_ptr[i] == 0.0)
            {
                status = false;
                goto wrapup;
            }
            else
            {
                alpha_ptr2[i] = (alpha_ptr2[i] - sum) / alpha_ptr[i];
            }
            /* next, the rows */
            sum = 0.0;
            for (k = 0; k <= i-1; k++)
            {
                sum += alpha_ptr[k] * alpha[k*NpM+j];
            }
            alpha_ptr[j] = alpha_ptr[j] - sum;
        }  /* end for j */
    }  /* end for i */
    
    /* Last, perform forward and backward substitution to find solution. */

    for (l = 0; l < M; l++)
    {
        for (i = 0; i < N; i++)
        {
            alpha_ptr = &alpha[i*NpM];
            sum = 0.0;
            for (j = 0; j <= i - 1; j++)
            {
                sum += alpha_ptr[j] * X[j][l];
            } /* end for j. */
            X[i][l] = alpha_ptr[N+l] - sum;
        } /* end for i */
        for (i = N-1; i >= 0; i--)
        {
            alpha_ptr = &alpha[i*NpM];
            sum = 0.0;
            for (j = i+1; j < N; j++)
            {
                sum += alpha_ptr[j] * X[j][l];
            }  /* end for j */
            if (alpha_ptr[i] == 0.0)
            {
                status = false;
                goto wrapup;
            }
            else
            {
                X[i][l] = (X[i][l] - sum) / alpha_ptr[i];
            }
        } /* end for i */
    } /* end for l */
    
  wrapup:
    
    return(status);
    
} 

} /* anonymous namespace */

/* This function creates a new matrix that is the inverse of the input matrix.  
   Currently, it asserts if no such matrix can be derived (i.e. if the matrix 
   is singular). */

hduMatrix hduMatrix::getInverse(bool &success) const
{
    hduMatrix output;    
    success = solveSystemViaLUD4x4(&m_elements[0][0], output);
    
    return output;
}

/* Decompose matrix and recover the upper 3x3 rotation matrix */
void hduMatrix::getRotationMatrix(hduMatrix &rotMat) const
{
    double M[4][4], Q[4][4], S[4][4];
    int i;

    // The polar decomposition code deals with column-major order of matrix
    // storage, so we need to perform a transpose
    for (i = 0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
        {
            M[i][j] = get(j, i);
        }
    }
        
    polar_decomp(M, Q, S);

    // The rotation matrix is contained in Q, but we need to transpose it
    // order to comply with the hduMatrix row-major storage convention
    for (i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            rotMat.set(i, j, Q[j][i]);
        }
    }
}

/* Decompose matrix and recover the upper 3x3 rotation matrix */
void hduMatrix::getRotationMatrix(double R[3][3]) const
{
    double M[4][4], Q[4][4], S[4][4];
    int i;

    // The polar decomposition code deals with column-major order of matrix
    // storage, so we need to perform a transpose
    for (i = 0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
        {
            M[i][j] = get(j, i);
        }
    }
        
    polar_decomp(M, Q, S);

    // The rotation matrix is contained in Q, but we need to transpose it
    // order to comply with the hduMatrix row-major storage convention
    for (i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            R[i][j] = Q[j][i];
        }
    }
}

void hduMatrix::getRotation(hduQuaternion &quat) const
{
    hduMatrix tempRot;
    getRotationMatrix(tempRot);

    quat.fromRotationMatrix(tempRot);
}

hduMatrix hduMatrix::createTranslation(
    const double x, const double y, const double z) 
{
    return createTranslation(hduVector3Dd(x,y,z));
}


hduMatrix hduMatrix::createTranslation(
    const hduVector3Dd &t) 
{
    hduMatrix mat;
    assert(mat.isIdentity());
    
    /* Set the last row to be the translation elements. */
    mat.set(3,0,t[0]);
    mat.set(3,1,t[1]);
    mat.set(3,2,t[2]);
    return mat;
}


hduMatrix hduMatrix::createScale(
    const double x, const double y, const double z) 
{
    return createScale(hduVector3Dd(x,y,z));
}


hduMatrix hduMatrix::createScale(
    const hduVector3Dd &s)
{
    hduMatrix mat;
    assert(mat.isIdentity());
    
    /* Set the diagonals to be the scale factors. */
    mat.set(0,0,s[0]);
    mat.set(1,1,s[1]);
    mat.set(2,2,s[2]);
    return mat;
}


hduMatrix hduMatrix::createRotation(
    const double x, const double y, const double z, const double angle)
{
    return createRotation(hduVector3Dd(x,y,z),angle);
}

/* This function creates a rotation around the given vector by the given angle 
   in radians.  Note that the guts of the routine is virtually a straight copy 
   from hduTransformMatrix's identical function. */

hduMatrix hduMatrix::createRotation(
    const hduVector3Dd &v, const double angle)
{  
    /* Vector must be non-zero. */
    assert(v.magnitude() > 0);
    
    hduMatrix mat;
    assert(mat.isIdentity());
    
    /* Create a new vector based on the old one, since we'll have to normalize 
       the vector.  We don't want to change the original input v, so we create 
       a new one and munge that instead. */
    hduVector3Dd axis = v;
    axis.normalize();

    /* This is a straight copy/replace from
       hduTransformMatrix::setRotation(const hduVector &axis, double angle). */
    double axisX = axis[0], axisY = axis[1], axisZ = axis[2];
    mat.set(0,0,axisX * axisX + cos(angle) * (1 - axisX * axisX));
    mat.set(0,1,axisX * axisY * (1 - cos(angle)) + axisZ * sin(angle));
    mat.set(0,2,axisZ * axisX * (1 - cos(angle)) - axisY * sin(angle));
    
    mat.set(1,0,axisX * axisY * (1 - cos(angle)) - axisZ * sin(angle));
    mat.set(1,1,axisY * axisY + cos(angle) * (1 - axisY * axisY));
    mat.set(1,2,axisY * axisZ * (1 - cos(angle)) + axisX * sin(angle));
    
    mat.set(2,0,axisZ * axisX * (1 - cos(angle)) + axisY * sin(angle));
    mat.set(2,1,axisY * axisZ * (1 - cos(angle)) - axisX * sin(angle));
    mat.set(2,2,axisZ * axisZ + cos(angle) * (1 - axisZ * axisZ));
    
    return mat;
}

/* Composes a transform matrix consisting of a rotation due to a
   quaternion representation and a translation */
hduMatrix hduMatrix::createRotationTranslation(const hduQuaternion &quat, 
                                               const hduVector3Dd &t)
{
    hduVector3Dd axis;
    double angle;
    quat.toAxisAngle(axis, angle);
    hduMatrix mat = hduMatrix::createRotation(axis, angle);
    mat.set(3,0,t[0]);
    mat.set(3,1,t[1]);
    mat.set(3,2,t[2]);

    return mat;
}
