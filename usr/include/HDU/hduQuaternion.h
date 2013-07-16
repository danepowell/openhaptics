/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduQuaternion.h

Description: 

  Implements quaternions (used to represent rotations).

*******************************************************************************/

#ifndef hduQuaternion_H_
#define hduQuaternion_H_

#include <HDU/hduVector.h>
#include <HDU/hduMatrix.h>

class hduQuaternion
{
public:

    hduQuaternion() : m_s(1), m_v(0, 0, 0) 
    {
    }

    hduQuaternion(double s, const hduVector3Dd &v) : m_s(s), m_v(v)
    {
    }

    explicit hduQuaternion(const double* q) : m_s(q[0]), m_v(&q[1])
    {
    }

    explicit hduQuaternion(double R[3][3]);

    explicit hduQuaternion(const hduMatrix& rotMat);

    hduQuaternion(const hduVector3Dd &axis, double angle);

    /* Convert to array. */
    operator const double *() const { return &m_s; }
    operator double *() { return &m_s; }   

    hduVector3Dd &v() { return m_v; }
    const hduVector3Dd &v() const { return m_v; }

    double &s() { return m_s; }
    const double s() const { return m_s; }

    void scale(double s);    
    void normalize();
    
    /* Converts to/from orthonormal rotation matrix and quaternion. */
    void toRotationMatrix(double R[3][3]) const;
    void fromRotationMatrix(const double R[3][3]);
    void toRotationMatrix(hduMatrix& rotMat) const;
    void fromRotationMatrix(const hduMatrix& rotMat);

    /* Converts to/from axis/angle representation and quaternion.
       Angle is specified in radians. */
    void toAxisAngle(hduVector3Dd &axis, double &angle) const;
    void fromAxisAngle(const hduVector3Dd &axis, double angle);
    
    hduQuaternion operator *= (const hduQuaternion &rhs); 

    hduQuaternion operator *= (double s); 

    double norm() const;

    hduQuaternion conjugate() const;

    hduQuaternion inverse() const;

private:

    /* ( Note that the array cast operator assumes that these two data
       members are always stored in this contiguous 
       order: s v0 v1 v2 ). */
    double m_s;
    hduVector3Dd m_v;
    
};

/******************************************************************************
 Multiply operator for quaternions.
******************************************************************************/
hduQuaternion operator * (const hduQuaternion &q1, const hduQuaternion &q2);
hduQuaternion operator * (double s, const hduQuaternion &q);
hduQuaternion operator * (const hduQuaternion &q, double s);
hduQuaternion operator + (const hduQuaternion &q1, const hduQuaternion &q2);
hduQuaternion operator - (const hduQuaternion &q1, const hduQuaternion &q2);

/******************************************************************************
 Output operator quaternions.
******************************************************************************/
inline std::ostream& operator<<(std::ostream& os, const hduQuaternion &q)
{
    return os << q.s() << " " << q.v();
}

/******************************************************************************
 Spherical linear interpolation.
*******************************************************************************/
hduQuaternion hduSlerp(const hduQuaternion& q1, const hduQuaternion& q2, double t);

#endif // hduQuaternion_H_

/*****************************************************************************/
