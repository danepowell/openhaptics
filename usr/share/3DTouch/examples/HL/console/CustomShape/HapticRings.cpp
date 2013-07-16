/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticRings.cpp

Description:

  HL custom constraint shape that haptically renders vertical and horizontal
  rings using local feature lines and points

*******************************************************************************/

#include <string.h>
#include "HapticRings.h"

/******************************************************************************
 HapticRings::HapticRings
******************************************************************************/
HapticRings::HapticRings() :
    m_radius(0.5)
{   
}


/******************************************************************************
 HapticRings::closestFeatures
 Finds the closest point to queryPt on the surface of the
 sphere.  Returns closest point in closestPt and surface
 normal at that point in closestNormal.
******************************************************************************/
bool HLCALLBACK HapticRings::closestFeatures(
    const HLdouble queryPt[3], 
    const HLdouble targetPt[3],
    HLgeom *geom,
    HLdouble closestPt[3],
    void *userdata)
{
    HapticRings *pThis = static_cast<HapticRings *>(userdata);

    hduVector3Dd queryPtV(queryPt);
    hduLineSegmentd segment;
    
    // Compute and add the closest horizontal ring segment.
    hduLineSegmentd segmentXY;
    pThis->closestXYTangentLine(queryPtV, segmentXY);
    
    // Compute and add the closest vertical ring segment.
    hduLineSegmentd segmentZY;
    pThis->closestZYTangentLine(queryPtV, segmentZY);
    
    // Only want one ring active at a time, so choose the segment that's
    // closer to the target.
    hduVector3Dd ptXY = segmentXY.project(hduVector3Dd(targetPt));
    HDdouble distSqXY = ptXY.distanceSqr(hduVector3Dd(targetPt));
    hduVector3Dd ptZY = segmentZY.project(hduVector3Dd(targetPt));
    HDdouble distSqZY = ptZY.distanceSqr(hduVector3Dd(targetPt));
    
    if (distSqXY < distSqZY)
    {       
        hlLocalFeature2dv(geom, HL_LOCAL_FEATURE_LINE, 
            segmentXY.getStartPoint(), segmentXY.getEndPoint());
        memcpy(closestPt, ptXY, sizeof(ptXY));
    }
    else
    {
        hlLocalFeature2dv(geom, HL_LOCAL_FEATURE_LINE, 
            segmentZY.getStartPoint(), segmentZY.getEndPoint());   
        memcpy(closestPt, ptZY, sizeof(ptZY));
    }
    
    // Also add point detents at the intersections of the rings.
    hlLocalFeature1dv(geom, HL_LOCAL_FEATURE_POINT, 
        hduVector3Dd(0, 0, pThis->getRadius()));   
    
    hlLocalFeature1dv(geom, HL_LOCAL_FEATURE_POINT, 
        hduVector3Dd(0, 0, -pThis->getRadius()));   

    return true;
}


/******************************************************************************
 HapticSphere::computeTangentLine
 Finds the closest surface feature(s) to queryPt.
******************************************************************************/
void HapticRings::closestXYTangentLine(const hduVector3Dd &queryPt,
                                       hduLineSegmentd &segment)
{
    hduVector3Dd ringNormal(queryPt[0], 0, queryPt[2]);
    ringNormal.normalize();

    hduVector3Dd ringPoint = ringNormal * getRadius();

    hduVector3Dd ringTangent(-ringNormal[2], 0, ringNormal[0]);

    segment.setStartPoint(ringPoint + getRadius() * ringTangent);
    segment.setEndPoint(ringPoint - getRadius() * ringTangent);
}

/******************************************************************************
 HapticSphere::computeTangentLine
 Finds the closest surface feature(s) to queryPt.
******************************************************************************/
void HapticRings::closestZYTangentLine(const hduVector3Dd &queryPt,
                                       hduLineSegmentd &segment)
{
    hduVector3Dd ringNormal(0, queryPt[1], queryPt[2]);
    ringNormal.normalize();

    hduVector3Dd ringPoint = ringNormal * getRadius();

    hduVector3Dd ringTangent(0, -ringNormal[2], ringNormal[1]);

    segment.setStartPoint(ringPoint + getRadius() * ringTangent);
    segment.setEndPoint(ringPoint - getRadius() * ringTangent);
}

/******************************************************************************/
