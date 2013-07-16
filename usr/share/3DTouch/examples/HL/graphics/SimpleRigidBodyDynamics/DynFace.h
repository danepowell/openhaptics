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

#if !defined(AFX_DYNFACE_H__ABDD0C97_E48A_4CBF_90C1_1379E56C4851__INCLUDED_)
#define AFX_DYNFACE_H__ABDD0C97_E48A_4CBF_90C1_1379E56C4851__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicsMath.h"
#include <vector>

#include "RigidBody.h"
#include "DynPlane.h"

class DynFace : public DynPlane
{
public:
    DynFace(RigidBody *rb);
    virtual ~DynFace();
        
    virtual hduVector3Dd *getNormal(void) { return body->getNormal(normalIndex); }
    virtual hduVector3Dd *getNormalOS(void) { return body->getNormalOS(normalIndex); }
    virtual hduVector3Dd *getFirstVertex(void) { return body->getVertex(vertices[0]); }
    hduVector3Dd *getVertex(int i) { return body->getVertex(vertices[i]); }
    hduVector3Dd *getVertexOS(int i) { return body->getVertexOS(vertices[i]); }
    int getNumVertices(void) { return vertices.size(); }

    void setNormalIndex(int n) { normalIndex = n; }
    void addVertexIndex(int i) { vertices.push_back(i); }
        
    void calculateNormal(hduVector3Dd *normal);
    void draw(void);

private:
    RigidBody *body;
    int normalIndex;
    std::vector<int> vertices;
};

#endif // !defined(AFX_DYNFACE_H__ABDD0C97_E48A_4CBF_90C1_1379E56C4851__INCLUDED_)

/******************************************************************************/
