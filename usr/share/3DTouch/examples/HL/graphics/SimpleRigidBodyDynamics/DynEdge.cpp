/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynEdge.cpp

Description:

  Edge of a rigid body used for collision detection/response.

*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#include "DynEdge.h"
#include "RigidBody.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DynEdge::DynEdge(RigidBody *rb, int i, int j) :
    mBody(rb)
{
    assert(rb);
    vertices[0] = i;
    vertices[1] = j;
}

DynEdge::~DynEdge()
{

}

hduVector3Dd *DynEdge::getFirstVertex(void)
{
    return mBody->getVertex(vertices[0]);
}

hduVector3Dd *DynEdge::getSecondVertex(void)
{
    return mBody->getVertex(vertices[1]);
}

hduVector3Dd *DynEdge::getNonColinearVertex(void)
{
    // assume the body has more than 2 vertices
    // assume that any third point will not be colinear
    int i = 0;
    while (i == vertices[0] || i == vertices[1])
        i++;
    
    return mBody->getVertex(i);
}

/******************************************************************************/
