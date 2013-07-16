/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynamicsWorld.h

Description:

  High level container for the rigid bodies.
*******************************************************************************/

#if !defined(AFX_DYNAMICSWORLD_H__C7485DA5_0EB8_4454_8E49_9F421B0E3ECC__INCLUDED_)
#define AFX_DYNAMICSWORLD_H__C7485DA5_0EB8_4454_8E49_9F421B0E3ECC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>
#include "RigidBody.h"
#include "Contact.h"
#include "Witness.h"
#include "OdeSolver.h"
#include "ContactList.h"

typedef std::map<HLuint, RigidBody*> BodyListT;
typedef std::list<Witness*> WitnessListT;

class DynamicsWorld  
{
public:
	DynamicsWorld();
	virtual ~DynamicsWorld();

	void advanceSimulation(double tPrev, double tCurr);
	void initSimulation(void);
	
	bool getDrawWitnesses(void) { return mDrawWitnesses; }
	void setDrawWitnesses(bool w) { mDrawWitnesses = w; }

	double getGravity(void) { return mGravity; }
	void setGravity(double g) { mGravity = g; }

	void drawWorld();
	void drawWorldHaptics();

	void addBox(hduVector3Dd pos, hduQuaternion angle, hduVector3Dd size, hduVector3Dd vel, hduVector3Dd angularVel, const char *name = 0);
	void addWall(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3, const char *name = 0);
	
	void activateMouseSpring(int x, int y);
	void moveMouseSpring(int x, int y);
	void deactivateMouseSpring(void);

    // Two modes: can either be touching the objects
    // in the scene or can take control of one object
    // with device
    enum HapticMode
    {
        TOUCH_OBJECTS,
        CONTROL_OBJECT
    };
    HapticMode getHapticMode() const {return hapticMode;}
    void setHapticMode(HapticMode m) {hapticMode = m;}

    HLuint getHapticControlObject() const {return hapticControlObject;}
    void setHapticControlObject(HLuint obj) {hapticControlObject = obj;}
    void setHapticControlObjectOffset(const hduVector3Dd& pos,
                                      const hduQuaternion& rot)
    {
        hapticControlObjectOffset = pos;
        hapticControlObjectRotOffset = rot;
    }
    
    void getHapticControlObjectOffset(hduVector3Dd& pos,
                                      hduQuaternion& rot)
    {
        pos = hapticControlObjectOffset;
        rot = hapticControlObjectRotOffset;
    }
    
    // get current position of body in simulation
    const hduVector3Dd& getBodyPosition(HLuint id) const;

    // get current orientation of body in simulation
    const hduQuaternion& getBodyOrientation(HLuint id) const;

private:
	BodyListT mBodies;
	ContactListT mContacts;
	WitnessListT mWitnesses;

	OdeSolverEuler odeSolver;
	
	bool mDrawWitnesses;
	double mGravity;

	nvectord x0;		// kSateSize * mBodies.size
	nvectord xFinal;	// kSateSize * mBodies.size

	RigidBody *mouseSpringBody;
	hduVector3Dd mouseSpringPos;
	
    HapticMode hapticMode;

    // if control object mode, id of object you
    // are controlling.
    HLuint hapticControlObject;
    hduVector3Dd hapticControlObjectOffset;
    hduQuaternion hapticControlObjectRotOffset;

	void arrayToBodies(nvectord &x);
	void bodiesToArray(nvectord &x);

	void computeForceAndTorque(double t, RigidBody *rb);
    void addHapticDeviceForce(RigidBody* rb);
	static bool dxdt(double t, nvectord &x, nvectord &xdot, void *userData);
	void ddtStateToArray(RigidBody *rb, double *xdot);
	
	bool findAllCollisions(void);
	void findAllContacts(bool &interPenetration);
	ESeparationState findSeparatingPlane(RigidBody &rbA, RigidBody &rbB, Witness &witness);
	ESeparationState findWitnessFromPrimaryFace(RigidBody &rbPri, RigidBody &rbSec, Witness &witness, double contactThreshold);
	ESeparationState findWitnessFromPrimaryEdge(RigidBody &rbPri, RigidBody &rbSec, Witness &witness, double contactThreshold);
	void deleteBodies(void);
	void deleteWitnesses(void);
	void deleteContacts(void);
};

#endif // !defined(AFX_DYNAMICSWORLD_H__C7485DA5_0EB8_4454_8E49_9F421B0E3ECC__INCLUDED_)
