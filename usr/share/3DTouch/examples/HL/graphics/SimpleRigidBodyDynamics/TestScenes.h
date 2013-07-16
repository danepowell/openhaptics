/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  TestScenes.h

Description:

  Routines to create various scenes (sets of rigid bodies with different
  initial conditions).

*******************************************************************************/

#ifndef _TestScenes_H
#define _TestScenes_H

void testEdgeEdgeCollision(void);
void testBigSmallFaceCollision(void);
void testVertexFaceCollision(void);
void testMultiBoxDrop(void);
void testSpins(void);
void testCubeMahem(void);
void testRolledBoxDrop(void);
void testBasicCollisions(void);
void testCubeTower(void);
void testEdgePlaneInvalidateCollision(void);
void testCatapult(void);
void testDominos(void);
void testReboundEdgeEdgeCollision(void);

#endif

/******************************************************************************/
