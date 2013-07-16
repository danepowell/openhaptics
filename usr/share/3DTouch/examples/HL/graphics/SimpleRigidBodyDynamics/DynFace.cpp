/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynFace.h

Description:

  Face of a rigid body used for collision detection/response.
*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#include "DynFace.h"

#if defined(WIN32)
# include "windows.h"
#endif

#if defined(WIN32) || defined(linux)
# include <GL/glu.h>
#elif defined(__APPLE__)
# include <OpenGL/glu.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DynFace::DynFace(RigidBody *rb)
{
    body = rb;
    normalIndex = -1;
}

DynFace::~DynFace()
{
}

void DynFace::calculateNormal(hduVector3Dd *normal)
{
#ifdef _DEBUG
    hduVector3Dd v0 = *getVertexOS(0);
    hduVector3Dd v1 = *getVertexOS(1);
    hduVector3Dd v2 = *getVertexOS(2);
        
    *normal = (v0 - v1).crossProduct(v1 - v2);
#endif

    *normal = (*getVertexOS(0) - *getVertexOS(1)).crossProduct(*getVertexOS(1) - *getVertexOS(2));
    *normal = *normal / normal->magnitude();
    assert(fabs(normal->magnitude() - 1) < 0.001);
}

void DynFace::draw(void)
{
    glBegin(GL_POLYGON);
    glNormal3dv(&(*getNormalOS())[0]);
    for (unsigned int i = 0; i < vertices.size(); i++)
        glVertex3dv(&(*getVertexOS(i))[0]);
        
    glEnd();

#ifdef DRAW_FACE_NORMALS
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 0.0, 0.0);

    hduVector3Dd vertAccum(3,0);
    for (i = 0; i < vertices.size(); i++)
        vertAccum += *getVertexOS(i);

    hduVector3Dd normalTail = vertAccum / (double)vertices.size();
    hduVector3Dd normalHead = normalTail + *getNormal();

    glBegin(GL_LINES);
    glVertex3dv(&normalTail[0]);
    glVertex3dv(&normalHead[0]);
    glEnd();

    glEnable(GL_LIGHTING);
#endif
}

/******************************************************************************/

