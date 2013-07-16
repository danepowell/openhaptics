/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynEdge.h

Description:

  Edge of a rigid body used for collision detection/response.

*******************************************************************************/

#if !defined(AFX_DYNEDGE_H__E5FDFB10_CB09_4017_A1F4_FA04AA3BDDA9__INCLUDED_)
#define AFX_DYNEDGE_H__E5FDFB10_CB09_4017_A1F4_FA04AA3BDDA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicsMath.h"
class RigidBody;

class DynEdge  
{
public:
    DynEdge(RigidBody *rb, int i, int j);
    virtual ~DynEdge();
    
    hduVector3Dd *getFirstVertex(void);
    hduVector3Dd *getSecondVertex(void);
    hduVector3Dd *getNonColinearVertex(void);
    
private:
    RigidBody *mBody;
    int vertices[2];
};

#endif // !defined(AFX_DYNEDGE_H__E5FDFB10_CB09_4017_A1F4_FA04AA3BDDA9__INCLUDED_)

/******************************************************************************/
