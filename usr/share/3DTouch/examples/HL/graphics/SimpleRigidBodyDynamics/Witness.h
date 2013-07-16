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

#if !defined(AFX_WITNESS_H__13B980AA_E249_4D34_AA31_111C0F884BD9__INCLUDED_)
#define AFX_WITNESS_H__13B980AA_E249_4D34_AA31_111C0F884BD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicsMath.h"
class RigidBody;
class DynFace;
#include "DynFreePlane.h"
#include "ContactList.h"

enum ESeparationState
{
    separationState_Unknown,
    separationState_Separation,
    separationState_Contact,
    separationState_Penetration
};

enum EWitnessType
{
    witnessType_None,
    witnessType_FaceOfPrimary,
    witnessType_EdgeOfPrimary
};

class Witness  
{
public:
    Witness();
    virtual ~Witness();

    ESeparationState checkSeparation(RigidBody &rbA, RigidBody &rbB);
    DynPlane *getSeparatingPlane(void);
        
    void establish(RigidBody *inRbPri, RigidBody *inRbSec, DynFace *inFace, double inDistance, double inContactThreshold);
    void establish(RigidBody *inRbPri, RigidBody *inRbSec, DynFreePlane &inFreePlane, double inDistance, double inContactThreshold);
        
    bool isBetterSeparation(double smallestDist);
    ESeparationState getState(void) { return state; }
    double getDistance(void) { return distance; }
    RigidBody *getRbPri(void) { return rbPri; }

    void improveSeparation(RigidBody *inRbPri, RigidBody *inRbSec, DynFace *inFace, double inDistance, double inContactThreshold);
    void improveSeparation(RigidBody *inRbPri, RigidBody *inRbSec, DynFreePlane &inFreePlane, double inDistance, double inContactThreshold);
        
    void updateFromBodies(void);
    void draw(void);
    void createContacts(ContactListT &inContacts);
        
private:
    RigidBody *rbPri;               // body containing face (or edge of freePlane)
    RigidBody *rbSec;               // secondary body
    DynFace *face;                  // pointer to a face of rbPri (if witnessType_FaceOfPrimary)

    // Note that freePlane is not a pointer so we don't have to worry about
    // dynamic allocation of DynFreePlane objects and our con/de-structors
    // remain simpler.
    DynFreePlane freePlane; // plane using edge of rbPri (if witnessType_EdgeOfPrimary)
    ESeparationState state; // does this witness separate the bodies?
    EWitnessType witnessType;// type of witness (DynFace of a body or uses a DynFreePlane)
    double distance;                // distance of separation/contact/penetration; negative indicates penetration

    bool freePlaneFailedPri;// if the freePlane no longer has all of rbPri's vertices below it

    double contactThreshold;// contactThreshold > d > -contactThreshold means d is considered in contact


    ESeparationState determineState(double inDistance);

};

#endif // !defined(AFX_WITNESS_H__13B980AA_E249_4D34_AA31_111C0F884BD9__INCLUDED_)

/******************************************************************************/
