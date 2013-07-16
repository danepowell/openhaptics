/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduPlane.h

Description: 

  Represents a geometric plane.

*******************************************************************************/

#ifndef hduPlane_H_
#define hduPlane_H_

#include <HDU/hduVector.h>
#include <HDU/hduLine.h>
#include <HDU/hduLineSegment.h>

/* Represents a geometric plane. */
template <class T>
class hduPlane
{
public:
    
    typedef T EltType;
    typedef hduVector3D<EltType> VecType;    

    enum IntersectResult
    {
        ResultNone = 0,
        ResultFront,
        ResultBack
    };

    /* Constructor. */
    explicit hduPlane(EltType a = 0.0,
                      EltType b = 1.0, 
                      EltType c = 0.0, 
                      EltType d = 0.0);

    /* Constructor. */
    hduPlane(const hduPlane &rhs) :
        m_normal(rhs.m_normal),
        m_d(rhs.m_d)
    {
    }

    /* Constructor. */
    hduPlane(const VecType &normal, EltType d) :
        m_normal(normal),
        m_d(d)
    {
    }

    hduPlane(const VecType &normal, 
             const VecType &pt);

    /* Constructor. */
    hduPlane(const VecType &pt1,
             const VecType &pt2, 
             const VecType &pt3);



    virtual ~hduPlane()
    {
    }

    /* Comparison. */
    bool operator==(const hduPlane &rhs) const
    {
        return m_normal == rhs.m_normal && m_d == rhs.m_d;
    }

    void setOffset(EltType d) { m_d = d; }

    /* Set plane to be located at position indicated by normal and point. */
    void setPlane(const VecType &normal, const VecType &pt);

    /* Returns A coefficient from plane equation. */
    EltType a() const { return m_normal[0]; }

    /* Returns B coefficient from plane equation. */
    EltType b() const { return m_normal[1]; }

    /* Returns C coefficient from plane equation. */
    EltType c() const { return m_normal[2]; }

    /* Returns D coefficient from plane equation. */
    EltType d() const { return m_d; }

    /* Returns normal vector of plane. */
    const VecType &normal() const { return m_normal; }

    /* Returns some point on the plane. Assumes that the normal is 
       unit length. */
    const VecType pointOnPlane() const { return -m_d * m_normal; }    

    /* Returns ax + by + cz + d. */
    virtual HDdouble perpDistance(const VecType &pt) const
    {
        return m_normal.dotProduct(pt) + m_d;
    }

    /* Returns vector perpendicular to point p. */
    virtual VecType perpVector(const VecType &pt) const
    {
        return (EltType) perpDistance(pt) * m_normal;
    } 
    
    /* Returns projection of point p onto plane. */
    virtual VecType projectPoint(const VecType &pt) const
    {
        return pt - perpVector(pt);
    }

    /* Returns projection of point p onto plane. Applies epsilon offset
       of point along normal. */
    virtual VecType projectPoint(const VecType &pt, const HDdouble epsilon) const
    {
        return projectPoint(pt) + epsilon * m_normal;
    }

    /* The following segment intersection routines test for plane intersection 
       with a line segment directed from start to end point.  Intersection tests 
       can be performed against the front, back or front and back of the plane. */
    virtual IntersectResult intersectSegmentFront(
        const hduLineSegment<T> &segment, HDdouble &t, const HDdouble epsilon) const;

    virtual IntersectResult intersectSegmentBack(
        const hduLineSegment<T> &segment, HDdouble &t, const HDdouble epsilon) const;

    virtual IntersectResult intersectSegmentFrontAndBack(
        const hduLineSegment<T> &segment, HDdouble &t, const HDdouble epsilon) const;

    /* The following line intersection routines test for plane intersection with
       an infinite line directed from pt1 through pt2.  Intersection tests can
       be performed against the front, back or front and back of the plane. */
    virtual IntersectResult intersectLineFront(
        hduLine<T> &line, HDdouble &t, const HDdouble epsilon) const;

    virtual IntersectResult intersectLineBack(
        hduLine<T> &line, HDdouble &t, const HDdouble epsilon) const;

    virtual IntersectResult intersectLineFrontAndBack(
        hduLine<T> &line, HDdouble &t, const HDdouble epsilon) const;

protected:

    VecType m_normal; // <a, b, c>
    EltType m_d;
};

typedef hduPlane<HDdouble> hduPlaned;
typedef hduPlane<HDfloat> hduPlanef;

#endif // hduPlane_H_

/*****************************************************************************/
