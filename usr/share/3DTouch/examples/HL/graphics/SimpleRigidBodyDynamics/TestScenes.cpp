/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  TestScenes.cpp

Description:

  Routines to create various scenes (sets of rigid bodies with different
  initial conditions).

*******************************************************************************/

#pragma warning( disable : 4786 )  // identifier was truncated to '255' 
                                   // the "you are using STL" warning
#include "SimpleRigidBodyDynamicsAfx.h"
#include "TestScenes.h"
#include "DynamicsWorld.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern DynamicsWorld *mWorld;

void startNewWorld(void);
void addWalls(double roomSize);
void addWall(double dist, int axis0, const char *name);


void addPairVertexFace(hduVector3Dd pos, hduVector3Dd size, hduVector3Dd vel);
void addPairBigSmall(hduVector3Dd pos, hduVector3Dd size0, hduVector3Dd size1, hduVector3Dd vel);
void addPairEdgeEdge(hduVector3Dd pos, hduVector3Dd size, hduVector3Dd vel, double theta);
void addMirroredPair(hduVector3Dd pos, hduQuaternion angle, hduVector3Dd size, hduVector3Dd vel, hduVector3Dd angularVel);

void testCubeMahem(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0, 0, 0);
    hduQuaternion angle;
    hduVector3Dd size(1, 1, 1);
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    for (int i = -1; i <= 1; i++ )
        for (int j = -1; j <= 1; j++)
            for (int k = -1; k <= 1; k++)
            {
                pos[0] = i*2;
                pos[1] = k*2;
                pos[2] = j*2;
                vel[0] = -i;
                vel[1] = -k;
                vel[2] = -j;
                mWorld->addBox(pos, angle, size, vel, angularVel);
            }
        
    //mWorld->setGravity(0);
    mWorld->initSimulation();
}

void testSpins(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduQuaternion angle;
    hduVector3Dd size(1,1,1);
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    pos[0] = -2;
    mWorld->addBox(pos, angle, size, vel, angularVel);

    pos[0] = 0;
    angularVel[1] = 1;
    mWorld->addBox(pos, angle, size, vel, angularVel);

    pos[0] = 2;
    vel[0] = -1;
    angularVel[1] = 0;
    angularVel[0] = 1.5;
    mWorld->addBox(pos, angle, size, vel, angularVel);

    vel[0] = 0;

    pos[0] = 0;
    pos[1] = 2;
    angularVel[0] = 0;
    angularVel[1] = 0;
    angularVel[2] = 1.2;
    mWorld->addBox(pos, angle, size, vel, angularVel);
        
    mWorld->setGravity(0);
    mWorld->initSimulation();

}

void testMultiEdgeEdgeCollision(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduVector3Dd size(0.5, 0.5, 0.5);
    hduVector3Dd vel(0,0,0);
        
    pos[0] = -1;
    vel[0] = 2;

    pos[1] = -4;
    addPairEdgeEdge(pos, size, vel, 35);
    pos[1] = -3;
    addPairEdgeEdge(pos, size, vel, 40);
    pos[1] = -2;
    addPairEdgeEdge(pos, size, vel, 45);
    pos[1] = -1;
    addPairEdgeEdge(pos, size, vel, 45);
    pos[1] = 0;
    addPairEdgeEdge(pos, size, vel, 50);
    pos[1] = 1;
    addPairEdgeEdge(pos, size, vel, -20);
    pos[1] = 2;
    addPairEdgeEdge(pos, size, vel, -163);
    pos[1] = 3;
    addPairEdgeEdge(pos, size, vel, 1);
    pos[1] = 4;
    addPairEdgeEdge(pos, size, vel, 3);

    mWorld->setGravity(0);
    mWorld->initSimulation();
}

void testReboundEdgeEdgeCollision(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduVector3Dd size(1,1,1);
    hduVector3Dd vel(0,0,0);
        
    pos[1] = -4;
    pos[0] = -4;
    vel[0] = 10;

    addPairEdgeEdge(pos, size, vel, 45);
    pos[0] = -1;
    addPairEdgeEdge(pos, size, vel, 40);

    mWorld->initSimulation();
}

void testEdgeEdgeCollision(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduVector3Dd size(1,1,1);
    hduVector3Dd vel(0,0,0);
        
    pos[0] = -1;
    vel[0] = 1;
    addPairEdgeEdge(pos, size, vel, 45);

    mWorld->setGravity(0);
    mWorld->initSimulation();
}

void addPairEdgeEdge(hduVector3Dd pos, hduVector3Dd size, hduVector3Dd vel, double theta)
{
    hduQuaternion angle;
    hduVector3Dd angularVel(0,0,0);
        
    angle.fromRotationMatrix(hduMatrix::createRotationAroundY(M_PI/180 * theta));
    mWorld->addBox(pos, angle, size, vel, angularVel);
        
    angle.fromRotationMatrix(hduMatrix::createRotationAroundZ(M_PI/180 * theta));
    pos[0] *= -1;
    vel[0] *= -1;
    mWorld->addBox(pos, angle, size, vel, angularVel);
}

void testBasicCollisions(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduVector3Dd size(1,1,1);
    hduVector3Dd size0(1.25,1.25,1.25);
    hduVector3Dd size1(0.75,0.75,0.75);
    hduVector3Dd vel(0,0,0);
        
    pos[1] = 3;
    pos[0] = -4;
    vel[0] = 2;
    addPairVertexFace(pos, size, vel);
        
    pos[1] = 1;
    pos[0] = -3;
    addPairBigSmall(pos, size0, size1, vel);

    hduQuaternion angle;
    hduVector3Dd angularVel(0,0,0);
    pos[1] = -1;
    pos[0] = -2;
    addMirroredPair(pos, angle, size, vel, angularVel);

    pos[1] = -3;
    pos[0] = -1;
    addPairEdgeEdge(pos, size, vel, 45);

    mWorld->setGravity(0);
    mWorld->initSimulation();
}

void testCatapult(void)
{
    startNewWorld();
    addWalls(10);
    double base = -5;
    hduVector3Dd pos(0,0,0);
    hduQuaternion angle;
    hduVector3Dd size(1.5,1.5,1.5);
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    // the frustrum
    pos[1] = base + size[1] / 2;
    base += size[1];
    mWorld->addBox(pos, angle, size, vel, angularVel);
                
    // the board
    size[1] = 0.5;
    pos[1] = base + size[1] / 2;
    base += size[1];
    size[0] = 5;
    mWorld->addBox(pos, angle, size, vel, angularVel);

    // the payload
    size[0] = 0.5;
    size[1] = 1;
    size[2] = 0.5;
    pos[0] = -2;
    pos[1] = base + size[1] / 2;
    mWorld->addBox(pos, angle, size, vel, angularVel);
        
    // the launcher
    size = hduVector3Dd(3,3,3);
    pos[0] = 3;
    pos[1] = 0;
    vel[1] = -1;
    mWorld->addBox(pos, angle, size, vel, angularVel);

    mWorld->initSimulation();
}

void testEdgePlaneInvalidateCollision(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduVector3Dd size(1,1,1);
    hduVector3Dd vel(0,0,0);
        
    hduQuaternion angle;
    angle.fromRotationMatrix(hduMatrix::createRotationAroundY(M_PI/180 * 45));
    hduVector3Dd angularVel(0,0,0);
    pos[0] = -2;
    vel[0] = 2;
    addMirroredPair(pos, angle, size, vel, angularVel);

    mWorld->setGravity(0);
    mWorld->initSimulation();
}

void testBigSmallFaceCollision(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduVector3Dd size0(1,1,1);
    hduVector3Dd size1(0.5,0.5,0.5);
    hduVector3Dd vel(0,0,0);
        
    pos[0] = -1;
    vel[0] = 1;
    addPairBigSmall(pos, size0, size1, vel);

    mWorld->setGravity(0);
    mWorld->initSimulation();
}

void addPairBigSmall(hduVector3Dd pos, hduVector3Dd size0, hduVector3Dd size1, hduVector3Dd vel)
{
    hduQuaternion angle;
    hduVector3Dd angularVel(0,0,0);

    mWorld->addBox(pos, angle, size0, vel, angularVel);
        
    pos[0] *= -1;
    vel[0] *= -1;
    mWorld->addBox(pos, angle, size1, vel, angularVel);
}




void testVertexFaceCollision(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduVector3Dd size(1,1,1);
    hduVector3Dd vel(0,0,0);

    pos[0] = -1;
    vel[0] = 1;
    addPairVertexFace(pos, size, vel);

    mWorld->setGravity(0);
    mWorld->initSimulation();
}

void addPairVertexFace(hduVector3Dd pos, hduVector3Dd size, hduVector3Dd vel)
{
    hduMatrix rotx;
    hduMatrix rotz;
        
    rotx = hduMatrix::createRotationAroundX(M_PI/180*45);
    rotz = hduMatrix::createRotationAroundZ(M_PI/180*54.75);

    hduQuaternion angle;
    hduVector3Dd angularVel(0,0,0);
        
    mWorld->addBox(pos, angle, size, vel, angularVel);
        
    angle.fromRotationMatrix(rotz * rotx);
    pos[0] *= -1;
    vel[0] *= -1;
    mWorld->addBox(pos, angle, size, vel, angularVel);
}

void addMirroredPair(hduVector3Dd pos, hduQuaternion angle, hduVector3Dd size, hduVector3Dd vel, hduVector3Dd angularVel)
{
    mWorld->addBox(pos, angle, size, vel, angularVel);
        
    pos[0] *= -1;
    vel[0] *= -1;
    angularVel[1] *= -1;
    angularVel[2] *= -1;
    // TODO: mirror the angle
    mWorld->addBox(pos, angle, size, vel, angularVel);
}

void testRolledBoxDrop(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduQuaternion angle;
    hduVector3Dd size(1,1,1);
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    angle.fromRotationMatrix(hduMatrix::createRotationAroundZ(M_PI/180*20));
    pos[0] = 0;
    mWorld->addBox(pos, angle, size, vel, angularVel);

    mWorld->initSimulation();
}

void testMultiBoxDrop(void)
{
    startNewWorld();
    addWalls(10);

    hduVector3Dd pos(0,0,0);
    hduQuaternion angle;
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    pos[0] = -2;
    hduVector3Dd size0(0.5,0.5,0.5);
    mWorld->addBox(pos, angle, size0, vel, angularVel);

    pos[0] = 0;
    hduVector3Dd size1(1,1,1);
    mWorld->addBox(pos, angle, size1, vel, angularVel);

    pos[0] = 2;
    hduVector3Dd size2(2,2,2);
    mWorld->addBox(pos, angle, size2, vel, angularVel);

    mWorld->initSimulation();
}

void testCubeTower(void)
{
    startNewWorld();
    addWalls(10);
        
    double floor = -5;
    double aboveFloor = 0;
    double initialSpaceBetween = 0;
    double towerHeight = 9;
    int numCubes = 10;

    hduVector3Dd pos(0,0,0);
    hduQuaternion angle;
    const double sizeCoord = (towerHeight / numCubes) - initialSpaceBetween;
    hduVector3Dd size(sizeCoord,sizeCoord,sizeCoord);
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    for (int i = 0; i < numCubes; i++)
    {
        pos[1] = floor + aboveFloor + (size[1] / 2) + i * (initialSpaceBetween + size[1]);
        angle.fromRotationMatrix(hduMatrix::createRotationAroundY(M_PI/180* (double)rand() / RAND_MAX * 360));
        mWorld->addBox(pos, angle, size, vel, angularVel);
    }

    pos[0] = -3;
    pos[1] = floor + aboveFloor + (size[1] / 2);
    hduQuaternion angleDefault;
    vel[0] = 2;
    mWorld->addBox(pos, angleDefault, size, vel, angularVel);

    mWorld->initSimulation();
}

void testDominos(void)
{
    startNewWorld();
    addWalls(10);
        
    double startPos = -3;
    double endPos = 4;
    double floor = -5;
    double spaceBetweenFraction = 0.5;
    double rowLength = endPos - startPos;
    int numBlocks = 10;

    hduVector3Dd pos(0,0,0);
    hduQuaternion angle;
    hduVector3Dd size(0,0,0);
    size[0] = (rowLength / numBlocks) * (1 - spaceBetweenFraction);
    size[1] = 3;
    size[2] = 2;
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    pos[1] = floor + size[1] / 2;
    for (int i = 0; i < numBlocks; i++)
    {
        pos[0] = startPos + i * (rowLength / numBlocks);
        mWorld->addBox(pos, angle, size, vel, angularVel);
    }

    pos[0] = -4;
    pos[1] = floor + 2;
    size[1] = size[2] = size[0] = 0.5;
    vel[0] = -15;
    vel[1] = 3;
    mWorld->addBox(pos, angle, size, vel, angularVel);

    mWorld->initSimulation();
}

void testPlateStack(void)
{
    startNewWorld();
    addWalls(10);
        
    double floor = -5;
    double aboveFloor = 0.5;
    double initialSpaceBetween = 0.2;
    double towerHeight = 9;
    int numCubes = 10;

    hduVector3Dd pos(0,0,0);
    hduQuaternion angle;
    const double sizeCoord = (towerHeight / numCubes) - initialSpaceBetween;
    hduVector3Dd size(sizeCoord,sizeCoord,sizeCoord);
    hduVector3Dd vel(0,0,0);
    hduVector3Dd angularVel(0,0,0);
        
    size[0] *= 5;
    size[2] *= 5;

    for (int i = 0; i < numCubes; i++)
    {
        pos[1] = floor + aboveFloor + (size[1] / 2) + i * (initialSpaceBetween + size[1]);
        angle.fromRotationMatrix(hduMatrix::createRotationAroundY(M_PI/180 * (double)rand() / RAND_MAX * 360));
        mWorld->addBox(pos, angle, size, vel, angularVel);
    }

    pos[0] = -3;
    pos[1] = floor + aboveFloor + (size[1] / 2);
    hduQuaternion angleDefault;
    vel[0] = 2;

    mWorld->initSimulation();
}


void startNewWorld(void)
{
    bool drawWitnesses = false;

    if (mWorld)
    {
        drawWitnesses = mWorld->getDrawWitnesses();
        delete mWorld;
    }

    mWorld = new DynamicsWorld();
    mWorld->setDrawWitnesses(drawWitnesses);
}

void addWalls(double roomSize)
{
    addWall(roomSize / 2, 0, "right wall");
    addWall(-roomSize / 2, 0, "left wall");
    addWall(roomSize / 2, 1, "top wall");
    addWall(-roomSize / 2, 1, "bottom wall");
    addWall(roomSize / 2, 2, "front wall");
    addWall(-roomSize / 2, 2, "back wall");
}

//
// Easy way of adding walls to make a square box
// Each wall will be dist from the origin, facing the origin
// Each vertex will be sqrt(3) * dist from origin
//
void addWall(double dist, int axis0, const char *name)
{
    int axis1;
    int axis2;

    if (axis0 == 0)
    {
        axis1 = 1;
        axis2 = 2;
    }
    else if (axis0 == 1)
    {
        axis1 = 2;
        axis2 = 0;
    }
    else
    {
        axis1 = 0;
        axis2 = 1;
    }
        
    hduVector3Dd v0(dist, dist, dist);
    hduVector3Dd v1(dist, dist, dist);
    hduVector3Dd v2(dist, dist, dist);
    hduVector3Dd v3(dist, dist, dist);
        
    if (dist > 0)
    {
        v1[axis2] = -dist;
        v2[axis1] = -dist;
        v2[axis2] = -dist;
        v3[axis1] = -dist;
    }
    else
    {
        v3[axis2] = -dist;
        v2[axis1] = -dist;
        v2[axis2] = -dist;
        v1[axis1] = -dist;
    }

    mWorld->addWall(v0, v1, v2, v3, name);
}
