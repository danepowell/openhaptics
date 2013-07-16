/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynFreePlane.cpp

Description:

  Free plane in dynamics scene (not part of a body).

*******************************************************************************/


#include "SimpleRigidBodyDynamicsAfx.h"
#include "DynFreePlane.h"
#include "DynEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DynFreePlane::DynFreePlane() :
    mNormal(0,0,0),
    mValid(false),
    mReverse(false)
{
}

DynFreePlane::~DynFreePlane()
{
}

void DynFreePlane::setEdges(DynEdge *ep, DynEdge *es)
{
    edgePri = ep;
    edgeSec = es;
        
    mValid = false;
    mReverse = false;
    recalculateNormal();
}

//
// Return true if a valid plane can be obtained or we have a valid
// normal from a previous calculation of the normal.
// If the two lines are parallel, a valid plane cannot be made.
// Note that we will only make mValid = false if the normal cannot
// be established when the edges are first set.
//
// This is because if the freePlane becomes
// invalid and no other witness is found, then
// we won't be able to rely on the witness from
// a previous state of separation when go to check
// for contact.  This will cause a crash!
// A plane, once created, cannot be allowed to become invalid.
//
bool DynFreePlane::calculateNormal(hduVector3Dd &v0, hduVector3Dd &v1, 
                                   hduVector3Dd &v2, hduVector3Dd &v3)
{
    hduVector3Dd newNormal = (v0 - v1).crossProduct(v2 - v3);
        
    double len = newNormal.magnitude();

    if (len > 0)
    {
        mNormal = newNormal / len;
        assert(fabs(mNormal.magnitude() - 1) < 0.001);
        return true;
    }
    else
        return false;
}

void DynFreePlane::recalculateNormal(void)
{
    if (!edgePri || !edgeSec)
    {
        mValid = false;
        return;
    }
        
    bool newNormalValid;
    if (!mReverse)
        newNormalValid = calculateNormal(
            *edgePri->getFirstVertex(),
            *edgePri->getSecondVertex(),
            *edgeSec->getFirstVertex(),
            *edgeSec->getSecondVertex());
    else
        newNormalValid = calculateNormal(
            *edgePri->getFirstVertex(),
            *edgePri->getSecondVertex(),
            *edgeSec->getSecondVertex(),
            *edgeSec->getFirstVertex());

    // if the normal was valid before, we will leave mValid = true
    if (newNormalValid)
        mValid = true;

}

hduVector3Dd *DynFreePlane::getNormal(void)
{
    if (mValid)
    {
        return &mNormal;
    }
    else
    {
        return NULL;
    }
}

hduVector3Dd *DynFreePlane::getFirstVertex(void)
{
    return edgePri->getFirstVertex();
}

void DynFreePlane::reverse(void)
{
    mNormal *= (double)-1;
        
    // We must remember to reverse the order of v2 and v3 when we recalculate
    mReverse = !mReverse;
}

/******************************************************************************/
