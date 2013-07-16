/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Witness.h

Description:

  Witness plane.  Used in collision detection.  Witness plane is a plane that
  separates two bodies.  Usually it is coincident to one of the faces of one
  of the bodies.  If both bodies are on opposite sides of the plane then there
  is no collision.  If no witness plane can be found for a pair of bodies then
  there is a collision.

*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#include "Witness.h"
#include "DynamicsMath.h"
#include "RigidBody.h"
#include "DynFace.h"
#include "DynFreePlane.h"
#include "Contact.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Witness::Witness() :
        rbPri(0),
        rbSec(0),
        face(0),
        state(separationState_Unknown),
        witnessType(witnessType_None),
        freePlaneFailedPri(false),
        contactThreshold(0)
{
}

Witness::~Witness()
{
}

ESeparationState Witness::checkSeparation(RigidBody &rbA, RigidBody &rbB)
{
    // Check to see if the witness has been initialized.
    if (state == separationState_Unknown)
        return separationState_Unknown;
        
    assert((&rbA == rbPri && &rbB == rbSec) ||
           (&rbB == rbPri && &rbA == rbSec));
        
    // First check to see if the rbSec has penetrated or made contact.
    double smallestDist = rbPri->distPlanePoly(getSeparatingPlane(), rbSec);

    if (witnessType == witnessType_EdgeOfPrimary)
    {
        // Make sure the plane doesn't intersect ourselves.

        // FIXME: we really only need to check one vertex from each of the
        // two faces adjacent to edgePri, not all the vertices in rbPri.
                
        freePlane.reverse();  // Flip it over to check ourselves.
        double distOfPrimary = rbPri->distPlanePoly(&freePlane, rbPri);
        freePlane.reverse();
                
        if (distOfPrimary > -0.001)
        {
            // This how it should normally be and we can just go on
            // our merry way.
        }
        else
        {
            // If this happens, we should be able to find another
            // separating plane.
            freePlaneFailedPri = true;

            state = separationState_Penetration;
            smallestDist = -9999;
        }
    }
        
    distance = smallestDist; // Remember the distance.
    // TODO: mark the time?

    state = determineState(distance);
        
    return state;
}

DynPlane *Witness::getSeparatingPlane(void)
{
    if (witnessType == witnessType_FaceOfPrimary)
        return face;
    else if (witnessType == witnessType_EdgeOfPrimary)
        return &freePlane;
    else
    {
        assert(false);
        return NULL;
    }
}

void Witness::establish(RigidBody *inRbPri, RigidBody *inRbSec, DynFace *inFace, double inDistance, double inContactThreshold)
{
    witnessType = witnessType_FaceOfPrimary;

    contactThreshold = inContactThreshold;

    rbPri = inRbPri;
    rbSec = inRbSec;
    face = inFace;
    freePlane.setEdges(NULL, NULL);
    freePlaneFailedPri = false;
    state = determineState(inDistance);
    distance = inDistance;
}

void Witness::establish(RigidBody *inRbPri, RigidBody *inRbSec, DynFreePlane &inFreePlane, double inDistance, double inContactThreshold)
{
    witnessType = witnessType_EdgeOfPrimary;

    contactThreshold = inContactThreshold;

    rbPri = inRbPri;
    rbSec = inRbSec;
    face = NULL;
    freePlane = inFreePlane;
    freePlaneFailedPri = false;
    state = determineState(inDistance);
    distance = inDistance;
}

void Witness::improveSeparation(RigidBody *inRbPri, RigidBody *inRbSec, DynFace *inFace, double inDistance, double inContactThreshold)
{
    assert(isBetterSeparation(inDistance));
    establish(inRbPri, inRbSec, inFace, inDistance, inContactThreshold);
}

void Witness::improveSeparation(RigidBody *inRbPri, RigidBody *inRbSec, DynFreePlane &inFreePlane, double inDistance, double inContactThreshold)
{
    assert(isBetterSeparation(inDistance));
    establish(inRbPri, inRbSec, inFreePlane, inDistance, inContactThreshold);
}

bool Witness::isBetterSeparation(double smallestDist)
{
    if (state == separationState_Unknown)
        return true;

    if (smallestDist > distance)
        return true;

    return false;
}

ESeparationState Witness::determineState(double inDistance)
{
    if (inDistance > contactThreshold)
        return separationState_Separation;
    else if (inDistance > -contactThreshold)
        return separationState_Contact;
    else
        return separationState_Penetration;

}

void Witness::updateFromBodies(void)
{
    if(witnessType == witnessType_EdgeOfPrimary)
        freePlane.recalculateNormal();
}

void Witness::draw(void)
{
    if (witnessType == witnessType_FaceOfPrimary)
    {
        // Draw an X across the face in blue.

        glColor3f(0.0, 0.0, 1.0);
        //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        //face->draw();
        glBegin(GL_LINES);
        int numVerts = face->getNumVertices();
        for (int i = 0; i < numVerts / 2; i++) // even
        {
            glVertex3dv(&(*face->getVertex(i))[0]);
            glVertex3dv(&(*face->getVertex(i+(numVerts / 2)))[0]);
        }
        glEnd();
    }
    else if (witnessType == witnessType_EdgeOfPrimary)
    {
        // Draw an X in blue where the plane is.

        glColor3f(0.0, 0.0, 1.0);

        hduVector3Dd v0(*freePlane.getFirstVertex());
        hduVector3Dd normal(*freePlane.getNormal());
        hduVector3Dd arbitrary(sqrt(3.0), sqrt(3.0), sqrt(3.0)); // a point outside the box
                
        hduVector3Dd acrossPlaneUp = normal.crossProduct(arbitrary);
        hduVector3Dd acrossPlaneOver = normal.crossProduct(acrossPlaneUp);
        hduVector3Dd corner(0, 0, 0);

        acrossPlaneUp *= (double) 0.5;
        acrossPlaneOver *= (double) 0.5;

        glBegin(GL_LINE_LOOP);
        corner = v0 + acrossPlaneUp;
        glVertex3dv(&corner[0]);
        corner = v0 + acrossPlaneOver;
        glVertex3dv(&corner[0]);
        corner = v0 - acrossPlaneUp;
        glVertex3dv(&corner[0]);
        corner = v0 - acrossPlaneOver;
        glVertex3dv(&corner[0]);
        glEnd();

        hduVector3Dd normalArrow = v0 + normal * 2.0;

        glBegin(GL_LINES);
        glVertex3dv(&v0[0]);
        glVertex3dv(&normalArrow[0]);

        corner = v0 + acrossPlaneUp;
        glVertex3dv(&corner[0]);
        corner = v0 - acrossPlaneUp;
        glVertex3dv(&corner[0]);
        corner = v0 + acrossPlaneOver;
        glVertex3dv(&corner[0]);
        corner = v0 - acrossPlaneOver;
        glVertex3dv(&corner[0]);
        glEnd();
    }
}

//
// Based on the witness, create a contact object(s)
// Currently, we are condensing (averaging) multiple contact
// points to a single.
//
// FIXME: averaging may cause mis-interpretation
// of collision states, such as when one vertex is colliding
// and another is separating but the average is in resting contact
//
void Witness::createContacts(ContactListT &inContacts)
{
    assert(rbPri);
    assert(rbSec);

    // Find all points that are in contact.
    VectorListT *vertices = rbSec->getVerticesWorld();
    VectorListT::const_iterator vi; // Constant because not modifying list
    double dist;
    double distAccum = 0;
    double distAverage;
    hduVector3Dd positionAccum(0,0,0);
    hduVector3Dd positionAverage(0,0,0);
    int numPointsInContact = 0;
    Contact *contact;

    for (vi = vertices->begin(); vi != vertices->end(); ++vi)
    {
        hduVector3Dd *vert = *vi;
        assert(vert != NULL);
        dist = rbPri->distPlanePoint(getSeparatingPlane(), vert);
        if (dist < contactThreshold)
        {
#ifdef AVOID_PENETRATION
            assert(dist > -contactThreshold);
#endif
            distAccum += dist;
            positionAccum += *vert;
            numPointsInContact++;
        }
    }

    // Usually we have 2 points of contact with EdgeOfPrimary witnesses,
    // but it could happen that a witness is developed
    // whereby 2 edges of one of the bodies collide with 1 or more
    // edges of the other body, which could result in some larger even number.
    assert(witnessType != witnessType_EdgeOfPrimary  || numPointsInContact % 2 == 0);

    // Create a single contact for all of the points.
    if (numPointsInContact > 0)
    {
        distAverage = distAccum / numPointsInContact;
        positionAverage = positionAccum / (double)numPointsInContact;
                
        dist = rbPri->distPlanePoint(getSeparatingPlane(), &positionAverage);

        assert(fabs(dist - distAverage) < 0.001);

        contact = new Contact();
        contact->a = rbSec;
        contact->b = rbPri;
        contact->p = positionAverage;
        contact->n = *getSeparatingPlane()->getNormal();
        //contact->ea   // not used
        //contact->eb   // not used
        //contact->vf = true;

        inContacts.push_back(contact);
    }
    else
    {
        // We should never be called to createContacts unless there is one
        // or more points in contact.
        assert(false);
    }
}

/******************************************************************************/
