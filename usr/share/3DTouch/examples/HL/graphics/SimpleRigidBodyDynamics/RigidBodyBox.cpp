/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  RigidBodyBox.cpp

Description:

  Box type rigid body.
*******************************************************************************/


#include "SimpleRigidBodyDynamicsAfx.h"
#include "RigidBodyBox.h"
#if defined(WIN32)
#include <windows.h>
#endif
#include "DynFace.h"
#include "DynEdge.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RigidBodyBox::RigidBodyBox(const char *name) :
    RigidBody(name),
    mSize(1,1,1)
{
    initialize();
}

RigidBodyBox::RigidBodyBox(hduVector3Dd inPosition, hduQuaternion inAngle, hduVector3Dd inSize, hduVector3Dd vel, hduVector3Dd angularVel, const char *name) :
    RigidBody(name),
    mSize(inSize)
{
    x = inPosition;
    q = inAngle;
    initialize();

    P = vel / massInv;
    q.normalize();
    q.toRotationMatrix(R);
    hduMatrix I = R * Ibody * R.getTranspose();
    L = I * angularVel;
}

RigidBodyBox::~RigidBodyBox()
{

}

void RigidBodyBox::initialize(void)
{
    // make the mass equal to the volume
    massInv = 1 / ( mSize[0] * mSize[1] * mSize[2] );

    // Determine Ibody
    double mfrac = 1 / (massInv * 12);
    Ibody(0,0) = mfrac * (mSize[1]*mSize[1] + mSize[2]*mSize[2]);
    Ibody(1,1) = mfrac * (mSize[0]*mSize[0] + mSize[2]*mSize[2]);
    Ibody(2,2) = mfrac * (mSize[0]*mSize[0] + mSize[1]*mSize[1]);
	
    // Note all other elements are 0 (initialized in RigidBody constructor)
	
    // The inverse is easy because the matrix is diagonal
    Ibodyinv(0,0) = 1. / Ibody(0,0);
    Ibodyinv(1,1) = 1. / Ibody(1,1);
    Ibodyinv(2,2) = 1. / Ibody(2,2);

    createFacesAndVertices();
}

// Setup our faces and vertices
void RigidBodyBox::createFacesAndVertices(void)
{
    addVertex(newVertex(+1, +1, +1)); // 0
    addVertex(newVertex(-1, +1, +1)); // 1
    addVertex(newVertex(+1, -1, +1)); // 2
    addVertex(newVertex(-1, -1, +1)); // 3
    addVertex(newVertex(+1, +1, -1)); // 4
    addVertex(newVertex(-1, +1, -1)); // 5
    addVertex(newVertex(+1, -1, -1)); // 6
    addVertex(newVertex(-1, -1, -1)); // 7
    assert(getVerticesObject()->size() == 8);
    assert(getVerticesWorld()->size() == 8);

    addBoxFace(0, 1, 3, 2);
    addBoxFace(4, 6, 7, 5);
    addBoxFace(0, 4, 5, 1);
    addBoxFace(2, 3, 7, 6);
    addBoxFace(0, 2, 6, 4);
    addBoxFace(1, 5, 7, 3);
    assert(getNormalsObject()->size() == 6);
    assert(getNormalsWorld()->size() == 6);
	
    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(0, 4);
    addEdge(1, 3);
    addEdge(1, 5);
    addEdge(2, 3);
    addEdge(2, 6);
    addEdge(3, 7);
    addEdge(4, 6);
    addEdge(4, 5);
    addEdge(6, 7);
    addEdge(5, 7);

    // check a couple of corners
    assert((*getVertexOS(0))[0] == mSize[0] / 2);
    assert((*getVertexOS(7))[2] == -mSize[2] / 2);
}

void RigidBodyBox::addBoxFace(int i, int j, int k, int l)
{
    DynFace *face = new DynFace(this);

    face->addVertexIndex(i);
    face->addVertexIndex(j);
    face->addVertexIndex(k);
    face->addVertexIndex(l);
	
    hduVector3Dd *normal = new hduVector3Dd;
    face->calculateNormal(normal);
    int normalIndex = addNormal(normal);

    face->setNormalIndex(normalIndex);
    addFace(face);
}

// x, y, z are +1 or -1 to indicate the face
hduVector3Dd *RigidBodyBox::newVertex(double x, double y, double z)
{
    hduVector3Dd *vertex = new hduVector3Dd;

    (*vertex)[0] = mSize[0] / 2 * x;
    (*vertex)[1] = mSize[1] / 2 * y;
    (*vertex)[2] = mSize[2] / 2 * z;

    return vertex;
}

void RigidBodyBox::draw(void)
{
    GLfloat matAmbDiff[] = { 1.0f, 0.2f, 0.2f, 1.0f };	// red
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDiff);

    glPushMatrix();
    glTranslated(x[0], x[1], x[2]);
    glMultMatrixd(R);

    drawGeom();
    glPopMatrix();
/*
  glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glColor3ub(255,255,255);
  glBegin(GL_LINES);
  glColor3ub(255,255,255);
  glVertex3dv(x);
  glColor3ub(0,0,0);
  glVertex3dv(x + torque);
  glColor3ub(255,255,0);
  glVertex3dv(x);
  glColor3ub(255,0,255);
  glVertex3dv(x + omega);
  glEnd();
  glPopAttrib();
*/
}

void RigidBodyBox::drawGeom(void)
{
    FaceListT *faces = getFaces();
    FaceListT::const_iterator fi; // constant because not modifying list
	
    for (fi = faces->begin(); fi != faces->end(); ++fi)
    {
        DynFace* face = *fi;
        assert(face != NULL);
		
        face->draw();
    }
}

void RigidBodyBox::drawHaptics(void)
{
    glPushMatrix();
    glTranslated(x[0], x[1], x[2]);
    glMultMatrixd(R);
    hlHinti(HL_SHAPE_FEEDBACK_BUFFER_VERTICES, 
            getFaces()->size() * getFaces()->front()->getNumVertices() + 100);
    hlMaterialf(HL_FRONT, HL_STATIC_FRICTION, 0.8f);
    hlMaterialf(HL_FRONT, HL_DYNAMIC_FRICTION, 0.8f);
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, getId());
    drawGeom();
    hlEndShape();
    glPopMatrix();
}
