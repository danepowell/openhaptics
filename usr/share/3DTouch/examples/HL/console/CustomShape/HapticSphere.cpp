/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticSphere.cpp

Description:
  
  HL custom shape that haptically renders a sphere.  Designed to be used
  with hlCallback as a custom shape.

*******************************************************************************/

#include "HapticSphere.h"
#include <iostream>
#include <string.h>

/******************************************************************************
 HapticSphere::HapticSphere
******************************************************************************/
HapticSphere::HapticSphere() :
    m_radius(0.5)
{   
}

/******************************************************************************
 HapticSphere::intersectSurface
 Intersects the line segment from startPt to endPt with the sphere. 
 Returns the closest point of intersection to the start point in intersectionPt.
 Returns the surface normal at intersectionPt in intersectionNormal.
 Returns which face (HL_FRONT or HL_BACK) is being touched in face.
 Returns true if an intersection occured.
******************************************************************************/
bool HapticSphere::intersectSurface(const HLdouble startPt[3], 
                                    const HLdouble endPt[3],
                                    HLdouble intersectionPt[3], 
                                    HLdouble intersectionNormal[3],
                                    HLenum *face,
                                    void *userdata)
{
    HapticSphere *pThis = static_cast<HapticSphere *>(userdata);    

    hduVector3Dd startPtV(startPt);
    hduVector3Dd endPtV(endPt);
    hduVector3Dd intersectionPtV;

    bool bFoundIntersection = false;
    bool bStartInside = pThis->isInside(startPtV);
    bool bEndInside = pThis->isInside(endPtV);

    // Don't proceed if the start and end are on the same side of the surface.
    if (bStartInside == bEndInside)
        return false;

    if (bEndInside && !bStartInside)
    {        
        // Intersect sphere from outside using start->end segment.
        bFoundIntersection = pThis->intersectSegmentOutIn(
            startPtV, endPtV, intersectionPtV);
        *face = HL_FRONT;
    }
    else
    {
        // Intersect sphere from inside using end->start segment.
        bFoundIntersection = pThis->intersectSegmentOutIn(
            endPtV, startPtV, intersectionPtV);
        *face = HL_BACK;
    }

    if (bFoundIntersection)
    {
        hduVector3Dd intersectionNormalV = intersectionPtV;
        intersectionNormalV.normalize();

        // If this is back face intersection, then reverse the normal.
        if (*face == HL_BACK)
        {
            intersectionNormalV *= -1;
        }

        intersectionPt[0] = intersectionPtV[0];
        intersectionPt[1] = intersectionPtV[1];
        intersectionPt[2] = intersectionPtV[2];
        
        intersectionNormal[0] = intersectionNormalV[0];
        intersectionNormal[1] = intersectionNormalV[1];
        intersectionNormal[2] = intersectionNormalV[2];        

        return true;
    }

    return false;
}

/******************************************************************************
 HapticSphere::closestSurfaceFeatures
 Finds the closest surface feature(s) to queryPt.
******************************************************************************/
bool HapticSphere::closestSurfaceFeatures(const HLdouble queryPt[3], 
                                          const HLdouble targetPt[3],
                                          HLgeom *geom,
                                          HLdouble closestPt[3],
                                          void *userdata)
{
    HapticSphere *pThis = static_cast<HapticSphere *>(userdata);

    // Return a plane tangent to the sphere as the closest 2D local feature.
    hduVector3Dd normal(queryPt);
    normal.normalize();

    hduVector3Dd point = normal * pThis->getRadius();

    hlLocalFeature2dv(geom, HL_LOCAL_FEATURE_PLANE, normal, point);
    memcpy(closestPt, point, sizeof(point));

    return true;
}

/******************************************************************************
 HapticSphere::intersectSegmentOutIn
 Helper method to intersect sphere with line segment.
******************************************************************************/
bool HapticSphere::intersectSegmentOutIn(
    const hduVector3Dd &startPt_LC,
    const hduVector3Dd &endPt_LC,
    hduVector3Dd &rIntersectionPt_LC) const
{
    hduVector3Dd p = startPt_LC;
    hduVector3Dd v = endPt_LC - startPt_LC;

    // Solve the intersection implicitly using the quadratic formula.
    double a = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    double b = 2 * (p[0]*v[0] + p[1]*v[1] + p[2]*v[2]);
    double c = p[0]*p[0] + p[1]*p[1] + p[2]*p[2] - m_radius * m_radius;

    double disc = b*b - 4*a*c;

    // The scale factor that must be applied to v so that p + nv is
    // on the sphere.
    double n;
    if(disc == 0.0)
    {
        n = (-b)/(2*a);
    }
    else if(disc > 0.0)
    {
        double posN = (-b + sqrt(disc))/(2*a);
        double negN = (-b - sqrt(disc))/(2*a);
        n = posN < negN ? posN : negN;
    }
    else
    {
        return false;
    }

    // n greater than one means that the ray defined by the two points
    // intersects the sphere, but beyond the end point of the segment.
    // n less than zero means that the intersection is 'behind' the
    // start point.
    if(n > 1.0 || n < 0.0)
    {
        return false;
    }

    rIntersectionPt_LC = p + n*v;
    return true;
}

/******************************************************************************
 HapticSphere::isInside
 Tests whether a point is inside or outside of the sphere.
******************************************************************************/
bool HapticSphere::isInside(const hduVector3Dd &testPt) const
{
    return testPt.magnitude() < m_radius;
}

/******************************************************************************/
