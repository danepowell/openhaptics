/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticSphere.h

Description:
  
  HL custom shape that haptically renders a sphere.  Designed to be used
  with hlCallback as a custom shape.

*******************************************************************************/

#ifndef HapticSphere_H_
#define HapticSphere_H_

#include <HL/hl.h>

#include <HDU/hduLineSegment.h>
#include <HDU/hduVector.h>

class HapticSphere
{
public:

    HapticSphere();

    /* Accessors for radius of sphere. */
    double getRadius() const { return m_radius; }
    void setRadius(double r) { m_radius = r; }
    
    /* Intersects the line segment from startPt to endPt with
       the sphere.  Returns the closest point of intersection 
       to the start point in intersectionPt.  Returns the
       surface normal at intersectionPt in intersectionNormal.
       Returns which face (HL_FRONT or HL_BACK) is being touched
       in face.
       Returns true if there is an intersection. */
    static bool HLCALLBACK intersectSurface(
        const HLdouble startPt[3], 
        const HLdouble endPt[3],
        HLdouble intersectionPt[3], 
        HLdouble intersectionNormal[3],
        HLenum *face,
        void *userdata);

    /* Finds the closest surface feature(s) to queryPt. */
    static bool HLCALLBACK closestSurfaceFeatures(
        const HLdouble queryPt[3], 
        const HLdouble targetPt[3],
        HLgeom *geom,
        HLdouble closestPt[3],
        void *userdata);

protected:

    /* Helper method to intersect sphere with line segment. */
    bool intersectSegmentOutIn(const hduVector3Dd &startPt_LC,
                               const hduVector3Dd &endPt_LC,
                               hduVector3Dd &rIntersectionPt_LC) const;

    bool isInside(const hduVector3Dd &testPt) const;

    void closestXYTangentLine(
        const hduVector3Dd &queryPt, 
        hduLineSegmentd &segment);

    void closestZYTangentLine(
        const hduVector3Dd &queryPt, 
        hduLineSegmentd &segment);

private:

    double m_radius;

};

#endif // HapticSphere_H_

/******************************************************************************/
