/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  RigidBodyWall.cpp

Description:

  Rigid body representing a wall (boundary) in the simulation.
*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#include "RigidBodyWall.h"
#include "DynFace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RigidBodyWall::RigidBodyWall(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3, const char *name) :
	RigidBody(name)
{
	initialize(v0, v1, v2, v3);
}

RigidBodyWall::~RigidBodyWall()
{

}


void RigidBodyWall::initialize(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3)
{
	// massInv is 0 because walls are fixed
	massInv = 0;

	createFacesAndVertices(v0, v1, v2, v3);
}

// Setup our faces and vertices
void RigidBodyWall::createFacesAndVertices(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3)
{
	addVertex(new hduVector3Dd(v0));
	addVertex(new hduVector3Dd(v1));
	addVertex(new hduVector3Dd(v2));
	addVertex(new hduVector3Dd(v3));
	assert(getVerticesObject()->size() == 4);
	assert(getVerticesWorld()->size() == 4);

	DynFace *face = new DynFace(this);

	face->addVertexIndex(0);
	face->addVertexIndex(1);
	face->addVertexIndex(2);
	face->addVertexIndex(3);
	
	hduVector3Dd *normal = new hduVector3Dd;
	face->calculateNormal(normal);
	int normalIndex = addNormal(normal);

	face->setNormalIndex(normalIndex);
	addFace(face);

	assert(getNormalsObject()->size() == 1);
	assert(getNormalsWorld()->size() == 1);

	//assert((*getVertexOS(0))[0] == mSize[0] / 2);
	//assert((*getVertexOS(7))[2] == -mSize[2] / 2);
}

void RigidBodyWall::draw(void)
{

	DynFace* face = *getFaces()->begin();

	GLfloat matAmbDiff[] = { 0.2f, 0.8f, 0.2f, 1.0f }; // green
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDiff);

    glPushMatrix();
    glTranslated(x[0], x[1], x[2]);
    glMultMatrixd(R);
	drawGeom();
    glPopMatrix();
}

void RigidBodyWall::drawGeom(void)
{
	DynFace* face = *getFaces()->begin();
	face->draw();
}

void RigidBodyWall::drawHaptics(void)
{
    glPushMatrix();
    glTranslated(x[0], x[1], x[2]);
    glMultMatrixd(R);
	DynFace* face = *getFaces()->begin();
    hlHinti(HL_SHAPE_FEEDBACK_BUFFER_VERTICES, face->getNumVertices() + 100);
    hlMaterialf(HL_FRONT, HL_STATIC_FRICTION, 0);
    hlMaterialf(HL_FRONT, HL_DYNAMIC_FRICTION, 0);
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, getId());
	drawGeom();
    hlEndShape();
    glPopMatrix();
}
