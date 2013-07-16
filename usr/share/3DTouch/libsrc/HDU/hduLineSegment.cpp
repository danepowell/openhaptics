/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduLineSegment.cpp

Description: 

  A geometric line segment

*******************************************************************************/

#include "hduAfx.h"

#include <HDU/hduLineSegment.h>
#include <float.h>

template <class T>
hduLineSegment<T>::hduLineSegment(const typename hduLine<T>::VecType &p1,
                                  const typename hduLine<T>::VecType &p2) :
    hduLine<T>(p1, p2)
{
    update();
}


template <class T>
HDdouble hduLineSegment<T>::projectToParametric(const typename hduLine<T>::VecType &pt) const
{
    // The parametric value of the projection of pt onto this line segment,
    // is given by the ratio of the length of the line segment with the
    // length of the vector (pt - p1) projected onto the line.
    
    if (length() == 0)
    {
        return 0;
    }
    else
    {
        HDdouble t = dotProduct(pt - hduLine<T>::m_p1, unitDirection()) / length();
        return hduClamp<HDdouble>(t, 0.0, 1.0);
    }
}

template <class T>
HDdouble hduLineSegment<T>::distPointSegment(const typename hduLine<T>::VecType &pt) const
{
    HDdouble t = projectToParametric(pt);
    typename hduLine<T>::VecType ptProjSeg = eval(t);
    return pt.distance(ptProjSeg);
}

template <class T>
HDdouble hduLineSegment<T>::distSqPointSegment(const typename hduLine<T>::VecType &pt) const
{
    HDdouble t = projectToParametric(pt);
    typename hduLine<T>::VecType ptProjSeg = eval(t);
    return pt.distanceSqr(ptProjSeg);
}

template <class T>
void hduLineSegment<T>::setStartPoint(const typename hduLine<T>::VecType &startPoint)
{
    hduLine<T>::setStartPoint(startPoint);
    update();
}


template <class T>
void hduLineSegment<T>::setEndPoint(const typename hduLine<T>::VecType &endPoint)
{
    hduLine<T>::setEndPoint(endPoint);
    update();
}


template <class T>
void hduLineSegment<T>::update()
{
    typename hduLine<T>::VecType vec = hduLine<T>::m_p2 - hduLine<T>::m_p1;
    m_length = vec.magnitude();
    if (m_length > 0.0)
    {
        m_unitDirection = vec / m_length;
    }
}

//-----------------------------------------------------------------------------
//
// Function:    distFromSegment
//
// Description: Determines the minimum distance between two finite line segs
//              segment1 = p1 + t(p2 - p1)
//              segment2 = p3 + s(p4 - p3)
//
// Parameters:  p1 and p2 describe a segment in the direction of p1 -> p2
//              p3 and p4 describe a segment in the direction of p3 -> p4
//
// Returns:     The return value of the function is the minimum distance
//              between these two segments.
//
//              The t and s parameters will indicate the parameters of
//              minimum distance for segment1 and segment2 respectively.
//
//-----------------------------------------------------------------------------
template <class T>
HDdouble hduLineSegment<T>::distFromSegment(
    const hduLineSegment &segment,
    HDdouble &t, HDdouble &s, 
    const HDdouble epsilon) const
{
    // Determine closest t and s parameters between the two lines
    HDdouble dist = hduLine<T>::distFromLine(segment, t, s, epsilon);   

    if ((t >= 0.0) && (t <= 1.0) && (s >= 0.0) && (s <= 1.0))
    {
        // min distance occurred on the segments
        return dist;
    }
    else
    {
        // Now determine the closest points on the two line segments
        typename hduLine<T>::VecType cp1, cp2;

        // min distance not on both segments, must choose minimum
        // distance from each of the segment endpoint to the other
        // segment
        HDdouble minDist = DBL_MAX;
        HDdouble dist;
        typename hduLine<T>::VecType tempPt;
        HDdouble tempParam;

        // Point 1 to line seg from points 3 to 4.
        tempParam = segment.projectToParametric(hduLine<T>::getStartPoint());
        tempPt = segment.evalSegment(tempParam);
        dist = hduLine<T>::getStartPoint().distance(tempPt);
        if (dist <= minDist)
        {
            minDist = dist;

            t = 0.0;
            cp1 = hduLine<T>::getStartPoint();

            s = tempParam;            
            cp2 = tempPt;
        }

        // Point 2 to line seg from points 3 to 4.
        tempParam = segment.projectToParametric(hduLine<T>::getEndPoint());  
        tempPt = segment.evalSegment(tempParam);
        dist = hduLine<T>::getEndPoint().distance(tempPt);
        if (dist <= minDist)
        {
            minDist = dist;

            t = 1.0;
            cp1 = hduLine<T>::getEndPoint();

            s = tempParam;            
            cp2 = tempPt;
        }

        // Point 3 to line seg from points 1 to 2.
        tempParam = projectToParametric(segment.getStartPoint());
        tempPt = evalSegment(tempParam);
        dist = segment.getStartPoint().distance(tempPt);
        if (dist <= minDist)
        {
            minDist = dist;

            t = tempParam;
            cp1 = tempPt;

            s = 0.0;            
            cp2 = segment.getStartPoint();
        }

        // Point 3 to line seg from points 1 to 2.
        tempParam = projectToParametric(segment.getEndPoint());
        tempPt = evalSegment(tempParam);
        dist = segment.getEndPoint().distance(tempPt);
        if (dist <= minDist)
        {
            minDist = dist;

            t = tempParam;
            cp1 = tempPt;

            s = 1.0;            
            cp2 = segment.getEndPoint();
        }

        return cp1.distance(cp2);
    }    
}


// Explicit template class instantiation
template class hduLineSegment<HDdouble>;
template class hduLineSegment<HDfloat>;


