/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Contact.h

Description:

  Contact between two rigid bodies.

*******************************************************************************/


#if !defined(AFX_CONTACT_H__0B4B12A3_37FD_4829_A63C_19AA1B012716__INCLUDED_)
#define AFX_CONTACT_H__0B4B12A3_37FD_4829_A63C_19AA1B012716__INCLUDED_

#if defined(WIN32)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

#include "DynamicsMath.h"
#include "RigidBody.h"

const double kCollidingThreshold = 0.01;

enum ECollidingState
{
    collidingState_Unknown,
    collidingState_Separating,
    collidingState_RestingContact,
    collidingState_Colliding
};

class Contact  
{
public:
    Contact();
    virtual ~Contact();

    ECollidingState colliding(void);
    void doCollision(double coefficientOfRestitution);

    RigidBody       *a;     // body containing vertex
    RigidBody       *b;     // body containing face
    hduVector3Dd    p;      // world-space vectex location
    hduVector3Dd    n;      // outwards pointing normal of face
};

#endif // !defined(AFX_CONTACT_H__0B4B12A3_37FD_4829_A63C_19AA1B012716__INCLUDED_)

/******************************************************************************/
