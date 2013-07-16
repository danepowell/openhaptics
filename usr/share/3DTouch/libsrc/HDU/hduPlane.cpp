/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
      
Module Name:
        
  hduPlane.cpp
          
Description: 
            
  Represents a geometric plane
              
*******************************************************************************/

#include "hduAfx.h"

#include <HDU/hduMath.h>
#include <HDU/hduPlane.h>
#include <HDU/hduLineSegment.h>

template <class T>
hduPlane<T>::hduPlane(EltType a,
                      EltType b,
                      EltType c,
                      EltType d) :
    m_normal(a, b, c),
    m_d(d)
{
    HDdouble length = m_normal.magnitude();
    if (length > 0.0)
    {
        m_d /= length;
        m_normal /= length;
    }
}


template <class T>
hduPlane<T>::hduPlane(const VecType &pt1,
                   const VecType &pt2,
                   const VecType &p3)
{
    const VecType v1 = pt2 - pt1;
    const VecType v2 = p3 - pt1;
    
    m_normal = v1.crossProduct(v2);
    m_normal.normalize();
    
    m_d = -(m_normal.dotProduct(pt1));
}


template <class T>
hduPlane<T>::hduPlane(const VecType &normal,
                   const VecType &pt)
{
    setPlane(normal, pt);
}


template <class T>
void hduPlane<T>::setPlane(const VecType &normal, const VecType &pt)
{
    m_normal = normal;
    m_normal.normalize();
    
    m_d = -(m_normal.dotProduct(pt));
}


//
// The following segment intersection routines test for plane intersection with
// a line segment directed from start to end point. Intersection tests can
// be performed against the front, back or front and back of the plane.
//
template <class T>
typename hduPlane<T>::IntersectResult hduPlane<T>::intersectSegmentFront(
    const hduLineSegment<T> &segment, HDdouble &t, const HDdouble epsilon) const
{     
    // No intersection if the start point is below the plane
    // Epsilon tolerance is used to allow for near-miss cases 
    HDdouble distToStartPt = perpDistance(segment.getStartPoint());    
    if (distToStartPt < 0.0 - epsilon)
    {
        return ResultNone;
    }

    // No intersection if the end point is above the plane
    // Epsilon tolerance is used to allow for near-miss cases 
    HDdouble distToEndPt = perpDistance(segment.getEndPoint());
    if (distToEndPt > 0.0 + epsilon)
    {        
        return ResultNone;
    }
    
    HDdouble ndotu = m_normal.dotProduct(segment.unitDirection());

    // If line is perp. with normal then line does not intersect
    if (hduIsEqual<T>(ndotu, 0.0, epsilon))
    {        
        return ResultNone;
    }

    HDdouble ndotv = ndotu * segment.length();
    t = (-distToStartPt) / ndotv;
    return ResultFront;
}

template <class T>
typename hduPlane<T>::IntersectResult hduPlane<T>::intersectSegmentBack(
    const hduLineSegment<T> &segment, HDdouble &t, const HDdouble epsilon) const
{
    // No intersection if the start point is above the plane
    // Epsilon tolerance is used to allow for near-miss cases 
    HDdouble distToStartPt = perpDistance(segment.getStartPoint());
    if (distToStartPt > 0.0 + epsilon)
    {
        return ResultNone;
    }

    // No intersection if the end point is above the plane
    // Epsilon tolerance is used to allow for near-miss cases 
    HDdouble distToEndPt = perpDistance(segment.getEndPoint());
    if (distToEndPt < 0.0 - epsilon)
    {        
        return ResultNone;
    }

    HDdouble ndotu = m_normal.dotProduct(segment.unitDirection());

    // If line is perp. with normal then line does not intersect
    if (hduIsEqual<HDdouble>(ndotu, 0.0, epsilon))
    {
        return ResultNone;
    }

    HDdouble ndotv = ndotu * segment.length();        
    t = (-distToStartPt) / ndotv;
    return ResultBack;
}


template <class T>
typename hduPlane<T>::IntersectResult hduPlane<T>::intersectSegmentFrontAndBack(
    const hduLineSegment<T> &segment, HDdouble &t, const HDdouble epsilon) const
{
    HDdouble distToStartPt = perpDistance(segment.getStartPoint());
    HDdouble distToEndPt = perpDistance(segment.getEndPoint());
    
    typename hduPlane<T>::IntersectResult result = ResultNone;

    // Test if the start is above the plane and end is below the plane.
    // Epsilon tolerance is used to allow for near-miss cases 
    if ((distToStartPt > 0.0 - epsilon) && (distToEndPt < 0.0 + epsilon))
    {        
        result = ResultFront;
    }
    // Test if the start is below the plane and end is above the plane.
    // Epsilon tolerance is used to allow for near-miss cases 
    else if ((distToStartPt < 0.0 + epsilon) && (distToEndPt > 0.0 - epsilon))
    {
        result = ResultBack;
    }
    else
    {
        return ResultNone;
    }

    // Now perform the standard intersection code
    HDdouble ndotu = m_normal.dotProduct(segment.unitDirection());
    
    // If line is perp. with normal then line does not intersect
    if (hduIsEqual<HDdouble>(ndotu, 0.0, epsilon))
    {
        return ResultNone;
    }
    
    HDdouble ndotv = ndotu * segment.length();
    
    t = (-distToStartPt) / ndotv;
    return result;
}


//
// The following line intersection routines test for plane intersection with
// an infinite line directed from pt1 through pt2. Intersection tests can
// be performed against the front, back or front and back of the plane.
//
template <class T>
typename hduPlane<T>::IntersectResult hduPlane<T>::intersectLineFront(
    hduLine<T> &line, HDdouble &t, const HDdouble epsilon) const
{
    const VecType v = line.direction();
    HDdouble ndotv = m_normal.dotProduct(v);
    
    // If line is perp. with normal then line does not intersect
    // Also make sure that the line is intersecting from the front
    if (hduIsEqual<HDdouble>(ndotv, 0.0, epsilon) || ndotv > 0.0)
    {
        t = 0;
        return ResultNone;
    }
    
    t = -perpDistance(line.getPointOnLine()) / ndotv;
    return ResultFront;
}


template <class T>
typename hduPlane<T>::IntersectResult hduPlane<T>::intersectLineBack(
    hduLine<T> &line, HDdouble &t, HDdouble epsilon) const
{
    const VecType v = line.direction();
    HDdouble ndotv = m_normal.dotProduct(v);
    
    // If line is perp. with normal then line does not intersect
    // Also make sure that the line is intersecting from the front
    if (hduIsEqual<T>(ndotv, 0.0, epsilon) || ndotv < 0.0)
    {
        t = 0;
        return ResultNone;
    }
        
    t = -perpDistance(line.getPointOnLine()) / ndotv;
    return ResultBack;
}


template <class T>
typename hduPlane<T>::IntersectResult hduPlane<T>::intersectLineFrontAndBack(
    hduLine<T> &line, HDdouble &t, const HDdouble epsilon) const
{
    const VecType v = line.direction();
    HDdouble ndotv = m_normal.dotProduct(v);
    
    typename hduPlane<T>::IntersectResult result = ResultNone;

    // If line is perp. with normal then line does not intersect
    if (hduIsEqual<T>(ndotv, 0.0, epsilon))
    {
        t = 0;
        return ResultNone;
    }
    
    // Check the directionality of the line relative to the plane
    if (ndotv < 0.0)
    {
        result = ResultFront;        
    }
    else
    {
        result = ResultBack;
    }

    t = -perpDistance(line.getPointOnLine()) / ndotv;
    return result;
}

// Explicit template class instantiation
template class hduPlane<HDdouble>;
template class hduPlane<HDfloat>;
