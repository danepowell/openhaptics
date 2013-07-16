/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduLine.cpp

Description: 

  A geometric line

*******************************************************************************/

#include "hduAfx.h"

#include <HDU/hduLine.h>
#include <HDU/hduLineSegment.h>
#include <float.h>

// 
// Tests whether this line is parallel within epsilon to the passed in line
//
template <class T>
bool hduLine<T>::isParallel(const hduLine &line, const HDdouble epsilon) const
{
    VecType lineDir1 = direction();
    VecType lineDir2 = line.direction();

    // These lines are parallel if the absolute value of the dot product of the
    // two vectors are close to 1.0 within some tolerance
    HDdouble numer = hduAbsValue(lineDir1.dotProduct(lineDir2));
    HDdouble denom = lineDir1.magnitude() * lineDir2.magnitude();

    if (denom <= epsilon * numer)  // <=, not <, in case both are 0
        return true;  // if degenerate, the app probably wants the special case

    HDdouble val = numer / denom;
    return hduIsEqual<HDdouble>(1.0, val, epsilon);    
}

//-----------------------------------------------------------------------------
//
// Function:    distFromLine
//
// Description: Determines the minimum distance between two infinite lines
//              Line1 = p1 + t(p2 - p1)
//              Line2 = p3 + s(p4 - p3)
//
// Parameters:  p1 and p2 describe a line in the direction of p1 -> p2
//              p3 and p4 describe a line in the direction of p3 -> p4
//
// Returns:     The return value of the function is the minimum distance
//              between these two lines.
//
//              The t and s parameters will indicate the parameters of
//              minimum distance for line1 and line2 respectively.
//-----------------------------------------------------------------------------
template <class T> 
HDdouble hduLine<T>::distFromLine(const hduLine &line,
                                  HDdouble &t, HDdouble &s, 
                                  const HDdouble epsilon) const
{
    VecType v = direction();
    VecType d = line.direction();
    VecType w = getPointOnLine() - line.getPointOnLine();
    
    // Case 1 applies if v is not parallel to d. Test this by comparing
    // the dot product of the two vectors to 1.0 within some tolerance
    if (!isParallel(line, epsilon))
    {
        double d_dot_v = d.dotProduct(v);
        double d_dot_d = d.dotProduct(d);
        double v_dot_v = v.dotProduct(v);
        double d_dot_w = d.dotProduct(w);
        double v_dot_w = v.dotProduct(w);

        double den = v_dot_v * d_dot_d - d_dot_v * d_dot_v;
        
        if (den)
        {
            t = (d_dot_v * d_dot_w - d_dot_d * v_dot_w) / den;
            s = (v_dot_v * d_dot_w - d_dot_v * v_dot_w) / den;
        }
        else
        {
            // Not sure how to handle a divide by zero case. Seems like
            // returning DBL_MAX is sufficient 
            return DBL_MAX;
        }
    }
    // Case 2 applies if v is parallel to d
    else 
    {
        // No unique solution exists, so just set t = 0 and let s be the
        // projection of line1(t) onto line2.

        t = 0;
        hduLineSegment<T> line1(m_p1, m_p2);
        hduLineSegment<T> line2(line.m_p1, line.m_p2);
        s = line2.projectToParametric(line1.eval(t));

        // Make sure we don't wind up with a wild result
        s = hduClamp<HDdouble>(s, 0.0, 1.0);
    }

    // Evaluate the position of the points on both lines and return the
    // distance between them.
    VecType c1 = eval(t);
    VecType c2 = line.eval(s);

    return c1.distance(c2);
}

// Explicit template class instantiation
template class hduLine<HDdouble>;
template class hduLine<HDfloat>;
