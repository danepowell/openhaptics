/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  RigidBodyWall.h

Description:

  Rigid body representing a wall (boundary) in the simulation.
*******************************************************************************/

#if !defined(AFX_RIGIDBODYWALL_H__E0BE3FB7_BEDE_45A5_8B04_AAA3B75F2769__INCLUDED_)
#define AFX_RIGIDBODYWALL_H__E0BE3FB7_BEDE_45A5_8B04_AAA3B75F2769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RigidBody.h"

class RigidBodyWall : public RigidBody  
{
public:
	RigidBodyWall(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3, const char *name = 0);
	virtual ~RigidBodyWall();

	virtual void draw();
	
	// walls never collide on their edges
	virtual bool noEdgeCollisions(void) { return true; }

    void drawHaptics();
    void drawGeom();

private:
	void initialize(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3);
	void createFacesAndVertices(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3);
};

#endif // !defined(AFX_RIGIDBODYWALL_H__E0BE3FB7_BEDE_45A5_8B04_AAA3B75F2769__INCLUDED_)
