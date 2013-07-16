/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduQuaternion.cpp

Description: 

  Implements quaternions (to represent rotations)

Quaternion slerp function is based on code from Jason Shankel
   * "Portions Copyright (C) Jason Shankel, 2000

*******************************************************************************/

#include "hduAfx.h"

#include <HDU/hduQuaternion.h>
#include <HDU/hduMath.h>

#include <assert.h>

hduQuaternion::hduQuaternion(double R[3][3])
{
    fromRotationMatrix(R);
}

hduQuaternion::hduQuaternion(const hduVector3Dd &axis, double angle)
{
    fromAxisAngle(axis, angle);
}


hduQuaternion::hduQuaternion(const hduMatrix& rotMat)
{
    fromRotationMatrix(rotMat);
}

hduQuaternion hduQuaternion::operator *= (const hduQuaternion &rhs)
{
    return (*this * rhs);
}

hduQuaternion hduQuaternion::operator *= (double s)
{
    return (s * (*this));
}

hduQuaternion operator * (const hduQuaternion &q1, const hduQuaternion &q2)
{    
    hduQuaternion result;
	result.s() = q1.s() * q2.s() - q1.v().dotProduct(q2.v());
	result.v() = q1.s() * q2.v() + q2.s() * q1.v() + q2.v().crossProduct(q1.v());
    return result;
}

hduQuaternion operator * (double s, const hduQuaternion &q)
{    
    return hduQuaternion(s * q.s(), s *q.v());
}

hduQuaternion operator * (const hduQuaternion &q, double s)
{    
    return hduQuaternion(s * q.s(), s *q.v());
}

hduQuaternion operator + (const hduQuaternion &q1, const hduQuaternion &q2)
{    
    hduQuaternion result;
	result.s() = q1.s() + q2.s();
	result.v() = q1.v() + q2.v();
    return result;
}

hduQuaternion operator - (const hduQuaternion &q1, const hduQuaternion &q2)
{    
    hduQuaternion result;
	result.s() = q1.s() - q2.s();
	result.v() = q1.v() - q2.v();
    return result;
}

void hduQuaternion::scale(double s)
{
	m_s *= s;
	m_v *= s;
}

void hduQuaternion::normalize()
{
    double lengthSqr = m_s * m_s + m_v.dotProduct(m_v);
    if (lengthSqr != 0)
    {
        scale(1.0 / sqrt(lengthSqr));
    }
}

double hduQuaternion::norm() const
{
    return m_s * m_s + m_v.dotProduct(m_v);
}

hduQuaternion hduQuaternion::conjugate() const
{
    return hduQuaternion(m_s, -1 * m_v);
}

hduQuaternion hduQuaternion::inverse() const
{
    double n = norm();
    if (n == 0)
    {
        return hduQuaternion(0, hduVector3Dd(0,0,0));
    }
    else
    {
        return 1/n * conjugate();
    }
}

// Produces a rotation matrix for row-major storage which
// is compatible with hduMatrix use
void hduQuaternion::toRotationMatrix(double R[3][3]) const
{
    double tx  = 2.0*m_v[0];
    double ty  = 2.0*m_v[1];
    double tz  = 2.0*m_v[2];
    double twx = tx*m_s;
    double twy = ty*m_s;
    double twz = tz*m_s;
    double txx = tx*m_v[0];
    double txy = ty*m_v[0];
    double txz = tz*m_v[0];
    double tyy = ty*m_v[1];
    double tyz = tz*m_v[1];
    double tzz = tz*m_v[2];

    R[0][0] = 1.0-(tyy+tzz);
    R[1][0] = txy-twz;
    R[2][0] = txz+twy;
    R[0][1] = txy+twz;
    R[1][1] = 1.0-(txx+tzz);
    R[2][1] = tyz-twx;
    R[0][2] = txz-twy;
    R[1][2] = tyz+twx;
    R[2][2] = 1.0-(txx+tyy);
}


// Produces a quaternion from a row-major 3x3 matrix. The 3x3
// can be extracted from the upper 3x3 of a 4x4 hduMatrix
void hduQuaternion::fromRotationMatrix(const double R[3][3])
{
    double trace = R[0][0]+R[1][1]+R[2][2];
    double root;

    double w, x, y, z;

    if ( trace > 0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = sqrt(trace+1.0);  // 2w
        w = 0.5*root;
        root = 0.5/root;  // 1/(4w)
        x = (R[1][2]-R[2][1])*root;
        y = (R[2][0]-R[0][2])*root;
        z = (R[0][1]-R[1][0])*root;
    }
    else
    {
        // |w| <= 1/2
        static int next[3] = { 1, 2, 0 };
        int i = 0;
        if ( R[1][1] > R[0][0] )
            i = 1;
        if ( R[2][2] > R[i][i] )
            i = 2;
        int j = next[i];
        int k = next[j];

        root = sqrt(R[i][i]-R[j][j]-R[k][k]+1.0);
        double* quat[3] = { &x, &y, &z };
        *quat[i] = 0.5*root;
        root = 0.5/root;
        w = (R[j][k]-R[k][j])*root;
        *quat[j] = (R[i][j]+R[j][i])*root;
        *quat[k] = (R[i][k]+R[k][i])*root;
    }

    m_v.set(x,y,z);
    m_s = w;
}

void hduQuaternion::toRotationMatrix(hduMatrix& rotMat) const
{
    double r[3][3];
    toRotationMatrix(r);
    rotMat.set(0, 0, r[0][0]);
    rotMat.set(0, 1, r[0][1]);
    rotMat.set(0, 2, r[0][2]);
    rotMat.set(0, 3, 0);
    rotMat.set(1, 0, r[1][0]);
    rotMat.set(1, 1, r[1][1]);
    rotMat.set(1, 2, r[1][2]);
    rotMat.set(1, 3, 0);
    rotMat.set(2, 0, r[2][0]);
    rotMat.set(2, 1, r[2][1]);
    rotMat.set(2, 2, r[2][2]);
    rotMat.set(2, 3, 0);
    rotMat.set(3, 0, 0);
    rotMat.set(3, 1, 0);
    rotMat.set(3, 2, 0);
    rotMat.set(3, 3, 1);
}

void hduQuaternion::fromRotationMatrix(const hduMatrix& rotMat)
{
    double r[3][3];
    r[0][0] = rotMat.get(0,0);
    r[1][0] = rotMat.get(1,0);
    r[2][0] = rotMat.get(2,0);
    r[0][1] = rotMat.get(0,1);
    r[1][1] = rotMat.get(1,1);
    r[2][1] = rotMat.get(2,1);
    r[0][2] = rotMat.get(0,2);
    r[1][2] = rotMat.get(1,2);
    r[2][2] = rotMat.get(2,2);
    fromRotationMatrix(r);
}

void hduQuaternion::toAxisAngle(hduVector3Dd &axis, double &angle) const
{
    double lenSquared = m_v.dotProduct(m_v);
    
    if (lenSquared > 0.0)
    {
        assert(m_s >= -1.0 && m_s <= 1.0);
        angle = 2.0*acos(hduClamp<double>(m_s, -1.0, 1.0));
        double invlen = 1.0 / sqrt(lenSquared);
        axis = invlen * m_v;
    }
    else
    {
        // If angle is 0 so axis doesn't matter, pick any
        angle = 0;
        axis = hduVector3Dd(1.0, 0.0, 0.0);
    }
}


void hduQuaternion::fromAxisAngle(const hduVector3Dd &axis, double angle)
{
    hduVector3Dd t = axis;
    t.normalize();

    double halfAngle = angle * 0.5;
    double sinTheta = sin(halfAngle);
    m_s = cos(halfAngle);
    m_v = sinTheta * t;
}

/******************************************************************************
 Spherical linear interpoaltion
*******************************************************************************/
hduQuaternion hduSlerp(const hduQuaternion& q1, const hduQuaternion& q2, double t)
{
	hduQuaternion q3;

    // calculate dot product of tq1 and q2 which is actually cos of angle between
    // the two
	double dot;
	dot = q1.v().dotProduct(q2.v()) + q1.s()*q2.s();

	// if (dot < 0), q1 and q2 are more than 90 degrees apart,
	// so we can invert one to reduce spinning
	if (dot < 0)
	{
		dot = -dot;
		q3 = -1*q2;
	}
	else
	{
		q3 = q2;
	}

	
	if (dot < 0.95f)
	{
		double angle = acos(dot);
		double sina,sinat,sinaomt;
		sina = sin(angle);
		sinat = sin(angle*t);
		sinaomt = sin(angle*(1-t));
		return (q1*sinaomt+q3*sinat) * (1/sina);
	}
	// if the angle is small, use linear interpolation
	else
	{
		return hduLerp(q1,q3,t);
	}
}
