/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  RigidBodyBox.h

Description:

  Box type rigid body.
*******************************************************************************/


#if !defined(AFX_RIGIDBODYBOX_H__81994125_556B_4C25_997E_CF217EBD785F__INCLUDED_)
#define AFX_RIGIDBODYBOX_H__81994125_556B_4C25_997E_CF217EBD785F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RigidBody.h"

class RigidBodyBox : public RigidBody  
{
public:
    RigidBodyBox(const char *name = 0);
    RigidBodyBox(hduVector3Dd inPosition, hduQuaternion inAngle, hduVector3Dd inSize, hduVector3Dd vel, hduVector3Dd angularVel, const char *name = 0);

    virtual ~RigidBodyBox();

    virtual void draw();
    virtual void drawHaptics();
    void drawGeom();

private:
    hduVector3Dd mSize;

    void initialize(void);
    void createFacesAndVertices(void);
    hduVector3Dd *newVertex(double x, double y, double z);
    void addBoxFace(int i, int j, int k, int l);
};

#endif // !defined(AFX_RIGIDBODYBOX_H__81994125_556B_4C25_997E_CF217EBD785F__INCLUDED_)
