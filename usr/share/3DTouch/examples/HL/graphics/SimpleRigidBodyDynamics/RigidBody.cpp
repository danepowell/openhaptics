/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  RigidBody.cpp

Description:

  Single rigid body in the simulation.

*******************************************************************************/


#include "SimpleRigidBodyDynamicsAfx.h"
#include "RigidBody.h"

#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/gl.h>
#elif defined(__APPLE__)
# include <OpenGL/gl.h>
#endif

#include <stdlib.h>

#include "DynFace.h"
#include "DynEdge.h"
#include "DynPlane.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RigidBody::RigidBody(const char *name) :
    massInv(0),
    x(0,0,0),
    q(),
    P(0,0,0),
    L(0,0,0),
    v(0,0,0),
    omega(0,0,0),
    force(0,0,0),
    torque(0,0,0)
{
    zeroMatrix(Ibody);
    zeroMatrix(Ibodyinv);
    zeroMatrix(Iinv);
    zeroMatrix(R);

    setName(name);

    shapeId = hlGenShapes(1);
}

RigidBody::~RigidBody()
{
    // delete our faces, edges, vertices, and normals
    deleteFaces();
    deleteEdges(edges);

    // Since nonParallelEdges is a list of pointers to some 
    // of the edge objects in "edges" (i.e. re-use of same
    // edge objects) we don't need to delete them again, just
    // clear the list.
    nonParallelEdges.clear();
    deleteVectorList(verticesWorld);
    deleteVectorList(verticesObject);
    deleteVectorList(normalsWorld);
    deleteVectorList(normalsObject);

    hlDeleteShapes(shapeId, 1);
}

void RigidBody::deleteFaces(void)
{
    FaceListT::const_iterator ci;
        
    for (ci = faces.begin(); ci != faces.end(); ++ci)
    {
        DynFace *o = *ci;
        assert(o != NULL);
        delete o;
    }

    faces.clear();
}

void RigidBody::deleteEdges(EdgeListT &edgeList)
{
    EdgeListT::const_iterator ci;
        
    for (ci = edgeList.begin(); ci != edgeList.end(); ++ci)
    {
        DynEdge *o = *ci;
        assert(o != NULL);
        delete o;
    }

    edgeList.clear();
}

void RigidBody::deleteVectorList(VectorListT &vectors)
{
    VectorListT::const_iterator ci;
        
    for (ci = vectors.begin(); ci != vectors.end(); ++ci)
    {
        hduVector3Dd *o = *ci;
        assert(o != NULL);
        delete o;
    }

    vectors.clear();
}

void RigidBody::setName(const char *name)
{
    if (name)
        strncpy(mName, name, 255);
    else
        mName[0] = '\0';
}

// Copy the state information into an array
void RigidBody::stateToArray(double *y)
{
    int i;
    for(i = 0; i < 3; i++)
        *y++ = x[i];

    // Quaternion is implemented as a
    // r = real,
    // i, j, k = imaginary vector
    *y++ = q.s();
    *y++ = q.v()[0];
    *y++ = q.v()[1];
    *y++ = q.v()[2];

    for(i = 0; i < 3; i++)
        *y++ = P[i];

    for(i = 0; i < 3; i++)
        *y++ = L[i];
}

// Copy information from an array into the state variables
void RigidBody::arrayToState(double *y)
{
    int i;
    for(i = 0; i < 3; i++)
        x[i] = *y++;

    q.s() = *y++;
    q.v()[0] = *y++;
    q.v()[1] = *y++;
    q.v()[2] = *y++;

    for(i = 0; i < 3; i++)
        P[i] = *y++;

    for(i = 0; i < 3; i++)
        L[i] = *y++;

    // Compute auxiliary variables
    q.normalize();
    q.toRotationMatrix(R);
    v = P * massInv;
    Iinv = R.getTranspose() * Ibodyinv * R;
    omega = Iinv * L;

    transformObjectToWorld();
}

void RigidBody::draw(void)
{
}

void RigidBody::drawHaptics(void)
{
}

// Return the velocity of a point on a rigid body
// Note that a ^ b is the cross product of a and b
hduVector3Dd RigidBody::pointVelocity(const hduVector3Dd &p)
{
    return v + (omega.crossProduct(p - x));
}

// Determine the distance from the face to a point
double RigidBody::distPlanePoint(DynPlane *plane, hduVector3Dd *point)
{
    return (*point - *plane->getFirstVertex()).dotProduct(*plane->getNormal());
}

//
// Determine the distance from the face to the polyhedron
// If the polyhedron is non-penetrating, the distance to the
// closest point is returned.
// Otherwise, the distance to the furthest penetrating point
// is returned and the value will be negative.
// 
double RigidBody::distPlanePoly(DynPlane *plane, RigidBody *poly)
{
    double dist, smallest;

    VectorListT *vertices = poly->getVerticesWorld();
    VectorListT::const_iterator vi; // constant because not modifying list
        
    vi = vertices->begin();
    smallest = distPlanePoint(plane, *vertices->begin());

    for (vi++; vi != vertices->end(); ++vi)
    {
        hduVector3Dd *vert = *vi;
        assert(vert != NULL);

        dist = distPlanePoint(plane, vert);
        if (dist < smallest)
            smallest = dist;
    }

    return(smallest);
}

// transform vertices and normals from object to world space
void RigidBody::transformObjectToWorld(void)
{
    unsigned int i;
    for (i = 0; i < verticesWorld.size(); i++)
    {
        *verticesWorld[i] =  *verticesObject[i] * R + x;
    }
        
    // normals only get rotated, not translated
    for (i = 0; i < normalsWorld.size(); i++)
    {
        assert(fabs(normalsObject[i]->magnitude() - 1) < 0.001);
        R.multDirMatrix(*normalsObject[i], *normalsWorld[i]);
        assert(fabs(normalsWorld[i]->magnitude() - 1) < 0.001);
    }
}

void RigidBody::addVertex(hduVector3Dd *vertex)
{
    verticesObject.push_back(vertex);
    verticesWorld.push_back(new hduVector3Dd(*vertex));
}

int RigidBody::addNormal(hduVector3Dd *normal)
{
    normalsObject.push_back(normal);
    normalsWorld.push_back(new hduVector3Dd(*normal));

    return normalsObject.size() - 1;
}

void RigidBody::addFace(DynFace *face)
{
    faces.push_back(face);
}

void RigidBody::addEdge(DynEdge *edge)
{
    edges.push_back(edge);

    // check to see if it is parallel to existing edges
    EdgeListT::const_iterator ci;
        
    for (ci = nonParallelEdges.begin(); ci != nonParallelEdges.end(); ++ci)
    {
        DynEdge *e = *ci;
        assert(e != NULL);
                
        // if parallel, dot product will be 0
        if ((*e->getFirstVertex() - *e->getSecondVertex()).dotProduct(
            *edge->getFirstVertex() - *e->getSecondVertex()) == 0)
            return;
    }
        
    nonParallelEdges.push_back(edge);
}

void RigidBody::addEdge(int i, int j)
{
    addEdge(new DynEdge(this, i, j));
}

// update force and torque based on force at point in world space
void RigidBody::applyForceAtPoint(const hduVector3Dd& inputForce, 
                                  const hduVector3Dd& point)
{
    torque += (point - x).crossProduct(inputForce);
    force += inputForce;
}

/******************************************************************************/

