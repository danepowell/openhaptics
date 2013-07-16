/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  RigidBody.h

Description:

  Single rigid body in the simulation.

*******************************************************************************/


#if !defined(AFX_RIGIDBODY_H__A482870B_1F37_4DBA_A58D_F142F8450377__INCLUDED_)
#define AFX_RIGIDBODY_H__A482870B_1F37_4DBA_A58D_F142F8450377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "DynamicsMath.h"
#include <HL/hl.h>

class DynFace;
class DynEdge;
class DynPlane;

typedef std::vector<DynFace*> FaceListT;
typedef std::vector<DynEdge*> EdgeListT;
typedef std::vector<hduVector3Dd*> VectorListT;

class RigidBody  
{
public:
    char mName[256];        // for debugging

    // Constant quantities
    double                  massInv;        // 1 / mass     
    hduMatrix       Ibody;          // body-space inertia tensor
    hduMatrix       Ibodyinv;       // inverse of Ibody

    // State variables
    hduVector3Dd            x;                      // x(t) position
    hduQuaternion   q;                      // q(t) orientation
    hduVector3Dd            P;                      // P(t) linear momentum
    hduVector3Dd            L;                      // L(t) angular momentum

    // Derived quantities (auxiliary variables)
    hduMatrix       Iinv;           // inverse of I, the inertia tensor
    hduMatrix       R;                      // R(t) orientation
    hduVector3Dd            v;                      // v(t) velocity
    hduVector3Dd            omega;          // angular velocity

    // Computed quantities
    hduVector3Dd            force;          // F(t)
    hduVector3Dd            torque;         // tao(t)

    RigidBody(const char *name = 0);
    virtual ~RigidBody();

    void deleteVectorList(VectorListT &vectors);
    void deleteFaces(void);
    void deleteEdges(EdgeListT &edgeList);

    void stateToArray(double *y);
    void arrayToState(double *y);

    virtual void draw();
    virtual void drawHaptics();

    virtual void addVertex(hduVector3Dd *vertex);
    virtual int addNormal(hduVector3Dd *normal);
    virtual void addFace(DynFace *face);
    virtual void addEdge(DynEdge *edge);
    virtual void addEdge(int i, int j);

    virtual FaceListT *getFaces(void) { return &faces; }
    virtual EdgeListT *getEdges(void) { return &edges; }
    virtual EdgeListT *getNonParallelEdges(void) { return &nonParallelEdges; }
    virtual VectorListT *getVerticesObject(void) { return &verticesObject; }
    virtual VectorListT *getVerticesWorld(void) { return &verticesWorld; }
    virtual VectorListT *getNormalsObject(void) { return &normalsObject; }
    virtual VectorListT *getNormalsWorld(void) { return &normalsWorld; }

    virtual hduVector3Dd *getNormal(int i) { return normalsWorld[i]; }
    virtual hduVector3Dd *getNormalOS(int i) { return normalsObject[i]; }
    virtual hduVector3Dd *getVertex(int i) { return verticesWorld[i]; }
    virtual hduVector3Dd *getVertexOS(int i) { return verticesObject[i]; }
        
    hduVector3Dd pointVelocity(const hduVector3Dd &p);
    double distPlanePoint(DynPlane *plane, hduVector3Dd *point);
    double distPlanePoly(DynPlane *plane, RigidBody *poly);
        
    void transformObjectToWorld(void); // transform vertices and normals from object to world space
        
    void setName(const char *name);

    virtual bool noEdgeCollisions(void) { return false; }

    HLuint getId() const 
    {
        return shapeId;
    }

    // update force and torque based on force at point
    void applyForceAtPoint(const hduVector3Dd& force, 
                           const hduVector3Dd& point);

private:
    FaceListT faces;
    EdgeListT edges;
    EdgeListT nonParallelEdges;
    VectorListT verticesWorld; // world space
    VectorListT verticesObject; // object space
    VectorListT normalsWorld;  // world space
    VectorListT normalsObject; // object space
    HLuint shapeId; // id for haptic rendering
};

#endif // !defined(AFX_RIGIDBODY_H__A482870B_1F37_4DBA_A58D_F142F8450377__INCLUDED_)

/******************************************************************************/
