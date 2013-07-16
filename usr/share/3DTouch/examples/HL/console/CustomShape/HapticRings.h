/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticRings.h

Description:

  HL custom constraint shape that haptically renders vertical and horizontal
  rings using local feature lines and points

*******************************************************************************/

#ifndef HapticRings_H_
#define HapticRings_H_

#include <HL/hl.h>

#include <HDU/hduLineSegment.h>
#include <HDU/hduVector.h>

class HapticRings
{
public:

    HapticRings();

    // Accessors for radius of sphere.
    double getRadius() const { return m_radius; }
    void setRadius(double r) { m_radius = r; }

    // Finds the closest surface feature(s) to queryPt.
    static bool HLCALLBACK closestFeatures(
        const HLdouble queryPt[3], 
        const HLdouble targetPt[3],
        HLgeom *geom,
        HLdouble closestPt[3],
        void *userdata);

protected:

    void closestXYTangentLine(
        const hduVector3Dd &queryPt, 
        hduLineSegmentd &segment);

    void closestZYTangentLine(
        const hduVector3Dd &queryPt, 
        hduLineSegmentd &segment);

private:

    double m_radius;

};

#endif // HapticRings_H_

/******************************************************************************/
