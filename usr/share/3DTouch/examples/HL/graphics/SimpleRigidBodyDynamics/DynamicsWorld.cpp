/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynamicsWorld.cpp

Description:

  High level container for the rigid bodies.
*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#include "DynamicsWorld.h"
#include "RigidBodyBox.h"
#include "RigidBodyWall.h"
#include "DynFace.h"
#include "DynEdge.h"
#include "UnProjectUtilities.h"

// Distance between bodies to be considered in contact
static const double kContactThreshold = 0.001;

static const double kMouseContactThreshold = 0.1;

static const double kCoefficientOfRestitutionDef = 0.7;

// the number of values (double) needed to store the state for one body
static const int kStateSize = 13;

static const double kGravityDef = -9.8;

static const double kDragLinear = 0.1;
static const double kDragAngular = 0.1;

void HLCALLBACK OnHapticDeviceButtonDown(HLenum event, HLuint object, 
                                     HLenum thread, HLcache *cache, 
                                     void *userdata);
void HLCALLBACK OnHapticDeviceButtonUp(HLenum event, HLuint object, 
                                     HLenum thread, HLcache *cache, 
                                     void *userdata);

extern int nHapticDeviceDOFInput;
extern int nHapticDeviceDOFOutput;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DynamicsWorld::DynamicsWorld() :
    mDrawWitnesses(false),
    mGravity(kGravityDef),
    mouseSpringBody(NULL),
    mouseSpringPos(0,0,0),
    hapticMode(TOUCH_OBJECTS),
    hapticControlObject(HL_OBJECT_ANY)
{
    // handler for button up event
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       OnHapticDeviceButtonUp, this);
}

DynamicsWorld::~DynamicsWorld()
{
    hlRemoveEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       OnHapticDeviceButtonUp);

    hlEnable(HL_PROXY_RESOLUTION);
    // delete our bodies
    deleteBodies();

    // delete our witnesses
    deleteWitnesses();

    // delete our contacts
    deleteContacts();
}

void DynamicsWorld::arrayToBodies(nvectord &x)
{
    BodyListT::const_iterator ci; // constant because not modifying list
    int i = 0;

    for (ci = mBodies.begin(); ci != mBodies.end(); ++ci)
    {
        RigidBody& rb = *((*ci).second);
        assert(&rb != NULL);
                
        rb.arrayToState(&x[i * kStateSize]);

        i++;
    }

    //
    // Update any dependent values
    //
        
    WitnessListT::const_iterator ciW;
        
    for (ciW = mWitnesses.begin(); ciW != mWitnesses.end(); ++ciW)
    {
        Witness *w = *ciW;
        assert(w != NULL);
                
        w->updateFromBodies();
    }
}

void DynamicsWorld::bodiesToArray(nvectord &x)
{
    BodyListT::const_iterator ci; // constant because not modifying list
    int i = 0;

    for (ci = mBodies.begin(); ci != mBodies.end(); ++ci)
    {
        RigidBody& rb = *((*ci).second);
        assert(&rb != NULL);
                
        rb.stateToArray(&x[i * kStateSize]);

        i++;
    }
}

//
// External forces on the body.
// This would include forces such as gravity and drag.
// Wind or other force fields would work in here as well.
//
// Note that the forces of collision are applied and resolved
// elsewhere.
//
// Resting contact forces would be applied later after the
// external forces are applied.
//
void DynamicsWorld::computeForceAndTorque(double t, RigidBody *rb)
{
    // start by reseting forces
    rb->force = hduVector3Dd(0,0,0);
    rb->torque = hduVector3Dd(0,0,0);
        
    // gravity
    if (rb->massInv != 0)
        rb->force[1] = mGravity / rb->massInv;

    // drag
    rb->force -= kDragLinear * rb->v;
        
    // angular drag (drag torque)
    rb->torque -= kDragAngular * rb->omega;
        
    // mouse spring
    if (rb == mouseSpringBody)
    {
        assert(rb->massInv != 0); // no mouse springs to fixed objects

        hduVector3Dd xdiff = rb->x - mouseSpringPos;
        hduVector3Dd vdiff = rb->v; // FIXME: shouldn't we subtract the mouse velocity?
                
        double springLength = 0;
        double dist = xdiff.magnitude();
                
        double ks = 50 / rb->massInv;
        double kd = 10 / rb->massInv;

        if (dist == 0)
            return;
                
        hduVector3Dd force = - ((ks * (dist - springLength)) + (kd * xdiff.dotProduct(vdiff) / dist)) * (xdiff / dist);
                
//      assert(norm(force) < 5);
        rb->force += force;
    }

    // force from haptic device
    addHapticDeviceForce(rb);
}

void DynamicsWorld::addHapticDeviceForce(RigidBody* rb)
{
    if (hapticMode == TOUCH_OBJECTS)
    {
        // if we are in contact with the object, and it is
        // not a wall (with massInv == 0), then apply a force
        // opposite the force applied to the haptic device for
        // this shape
        HLboolean isTouching;
        hlGetShapeBooleanv(rb->getId(), HL_PROXY_IS_TOUCHING, &isTouching);
        if (isTouching && rb->massInv != 0)
        {
            hduVector3Dd force;
            hlGetShapeDoublev(rb->getId(), HL_REACTION_FORCE, force);
            hduVector3Dd contactPos;
            hlGetDoublev(HL_PROXY_POSITION, contactPos);
            const double deviceForceToSimForceScale = 10;
            rb->applyForceAtPoint(-deviceForceToSimForceScale * force, contactPos);
        }
    }
    else
    {
        // control object mode, apply spring force and torque to
        // the object to move it to the haptic device position
        if (rb->getId() == hapticControlObject)
        {
            // linear spring force between haptic device and body
            hduVector3Dd devicePos;
            hlGetDoublev(HL_DEVICE_POSITION, devicePos);
            devicePos += hapticControlObjectOffset;
            const double ks = 100;
            const double kd = 5;
            const double xbase = .2;

            // A realistic system would look at the penetratino
            // distance or reaction force into the object as a measure
            // of how much force to apply.  Since we don't do any real collision
            // detection here, just subtract by some arbitrary amount to
            // keep smaller objects stable.
            hduVector3Dd xdiff = devicePos - rb->x;
            if (xdiff.magnitude() < xbase)
                xdiff = hduVector3Dd(0,0,0);
            else
            {
                xdiff -= xdiff/xdiff.magnitude()*xbase;
            }

            hduVector3Dd addForce = ks*xdiff;
            rb->force += addForce;
            double xdiffsq = xdiff.dotProduct(xdiff);

            if (xdiffsq > 0.000001)
            {
                hduVector3Dd dampForce = 
                    kd * xdiff.dotProduct(rb->v)/xdiffsq * xdiff;
                if (dampForce.magnitude() < addForce.magnitude())
                    rb->force -= dampForce;
            }

            if (nHapticDeviceDOFInput == 6)
            {

                // spring torque between device and body
                // only if we actually get 6DOF input from the device
                hduQuaternion deviceRot;
                hlGetDoublev(HL_DEVICE_ROTATION, deviceRot);
                deviceRot.normalize();
                rb->q.normalize();

                // find rotation that takes body orientation to device orientation
                hduQuaternion delta = deviceRot * hapticControlObjectRotOffset 
                    * rb->q.inverse();

                // convert to torque - quaternion represents rotation
                // of angle theta about axis w is [cos(theta/2), w*sin(theta/2)]
                // multiplying 2 * delta.s * delta.v gives: 
                //      2*cos(theta/2)*sin(theta/2)*w
                // and using sinA*sinB formula 
                //      2*cos(theta/2)*sin(theta/2) = sin(theta)
                // so we end up with 
                //      2 * delta.s * delta.v = sin(theta)*w
                // which is desired torque
                hduVector3Dd torque = ( 2 *delta.s()) * delta.v();
                double ksang = 3;
                double kdang = 2;
                if (nHapticDeviceDOFOutput == 6)
                {
                    // for 6DOF want less damping since users resistance
                    // to torque naturally damps and extra damping adds drag
                    ksang = 15;
                    kdang = 0.1;
                }
                hduVector3Dd addTorque = ksang*torque;
                
                rb->torque += addTorque;
                double torquesq = torque.dotProduct(torque);

                if (torquesq > 0.000001)
                {
                    hduVector3Dd dampForce = torque.dotProduct(rb->omega)/torquesq * torque;
                    hduVector3Dd dampTorque = kdang * dampForce;
                    if (dampTorque.magnitude() < addTorque.magnitude())
                    {
                        rb->torque -= dampTorque;
                    }
                }
            }
        }
    }
}

bool DynamicsWorld::dxdt(double t, nvectord &x, nvectord &xdot, void *userData)
{
    DynamicsWorld *pThis = static_cast<DynamicsWorld *>(userData);
    assert(pThis);
        
    // Put data in x[] into mBodies
    pThis->arrayToBodies(x);
        
    bool interPenetration;
    pThis->findAllContacts(interPenetration);

    // FIXME: when penetration occurs, we should tell the solver to
    // search back in time for the contact before interpenetration occured
#ifdef AVOID_PENETRATION
    assert(!interPenetration);
#endif

    if (pThis->findAllCollisions())
    {
        pThis->bodiesToArray(x);
        //return true;
    }

    BodyListT::const_iterator ci; // constant because not modifying list
    int i = 0;

    for (ci = pThis->mBodies.begin(); ci != pThis->mBodies.end(); ++ci)
    {
        RigidBody& rb = *((*ci).second);
        assert(&rb != NULL);
                
        pThis->computeForceAndTorque(t, &rb);
        pThis->ddtStateToArray(&rb, &xdot[i * kStateSize]);

        i++;
    }

    return false;
}

void DynamicsWorld::ddtStateToArray(RigidBody *rb, double *xdot)
{
    int i;

    // copy d/dt x(t) = v(t) into xdot
    for(i=0; i<3; i++)
        *xdot++ = rb->v[i];
    hduQuaternion omegaq(0, rb->omega);
    hduQuaternion qdot = 0.5 * (omegaq * rb->q);
    *xdot++ = qdot.s();
    *xdot++ = qdot.v()[0];
    *xdot++ = qdot.v()[1];
    *xdot++ = qdot.v()[2];

    // d/dt P(t) = F(t)
    for(i=0; i<3; i++)
        *xdot++ = rb->force[i];
        
    // d/dt L(t) = tao(t)
    for(i=0; i<3; i++)
        *xdot++ = rb->torque[i];
}

void DynamicsWorld::advanceSimulation(double tPrev, double tCurr)
{
    // copy xFinal back to x0
    for(unsigned int i=0; i<kStateSize * mBodies.size(); i++)
        x0[i] = xFinal[i];

    //ode(x0, xFinal, kStateSize * mBodies.size(), tPrev, tCurr, dxdt);
    odeSolver.solve(x0, xFinal, tPrev, tCurr, dxdt, this);
        
    // copy d/dt X(tNext) into state variables
    arrayToBodies(xFinal);
}

void DynamicsWorld::initSimulation(void)
{
    int vecSize = kStateSize * mBodies.size();
    x0.resize(vecSize);     
    xFinal.resize(vecSize); 
        
    odeSolver.setSize(vecSize);

    //initStates();
    bodiesToArray(xFinal);

    // arrayToBodies will calculate derived quantities
    arrayToBodies(xFinal);

    // create the list of witnesses using empty witnesses
    unsigned int bodyPairsCount = mBodies.size() * (mBodies.size() - 1) / 2 ; 
    while(mWitnesses.size() < bodyPairsCount)
        mWitnesses.push_back(new Witness());
    while(mWitnesses.size() > bodyPairsCount)
    {
        delete *mWitnesses.end();
        mWitnesses.erase(mWitnesses.end());
    }
}

void DynamicsWorld::drawWorld()
{
    BodyListT::const_iterator ci; // constant because not modifying list

    for (ci = mBodies.begin(); ci != mBodies.end(); ++ci)
    {
        RigidBody& rb = *((*ci).second);
        assert(&rb != NULL);
                
        rb.draw();
    }
        
    if (mouseSpringBody)
    {
        glColor3f(0.5f, 0.5f, 0.7f);
        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        glVertex3dv(&(mouseSpringBody->x)[0]);
        glVertex3dv(&mouseSpringPos[0]);
        glEnd();        
        glEnable(GL_LIGHTING);
    }

    if (!mDrawWitnesses)
        return;

    WitnessListT::const_iterator ciW; // constant because not modifying list
        
    glDisable(GL_LIGHTING);
    for (ciW = mWitnesses.begin(); ciW != mWitnesses.end(); ++ciW)
    {
        Witness& witness = **ciW;
        assert(&witness != NULL);
                
        witness.draw();
    }
    glEnable(GL_LIGHTING);

}


void DynamicsWorld::drawWorldHaptics()
{
    BodyListT::const_iterator ci; // constant because not modifying list

    for (ci = mBodies.begin(); ci != mBodies.end(); ++ci)
    {
        RigidBody& rb = *((*ci).second);
        assert(&rb != NULL);
                
        rb.drawHaptics();
    }
}

void DynamicsWorld::addWall(hduVector3Dd v0, hduVector3Dd v1, hduVector3Dd v2, hduVector3Dd v3, const char *name)
{
    RigidBody *rb = new RigidBodyWall(v0, v1, v2, v3, name);
    mBodies.insert(BodyListT::value_type(rb->getId(), rb));
}

void DynamicsWorld::addBox(hduVector3Dd pos, hduQuaternion angle, hduVector3Dd size, hduVector3Dd vel, hduVector3Dd angularVel, const char *name)
{
    RigidBody *rb = new RigidBodyBox(pos, angle, size, vel, angularVel, name);
    mBodies.insert(BodyListT::value_type(rb->getId(), rb));

    // add a callback to handle button down while touching this shape
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, rb->getId(), HL_COLLISION_THREAD, 
                       OnHapticDeviceButtonDown, this);
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, rb->getId(), HL_CLIENT_THREAD, 
                       OnHapticDeviceButtonDown, this);
}

// returns true if a discontinuity occured
bool DynamicsWorld::findAllCollisions(void)
{
    bool hadCollisionInLoop;
    bool hadDiscontinuity = false;
    ECollidingState collidingState;
        
    do {
        hadCollisionInLoop = false;
                
        ContactListT::const_iterator ci; // constant because not modifying list

        for (ci = mContacts.begin(); ci != mContacts.end(); ++ci)
        {
            Contact& c = **ci;
            assert(&c != NULL);
                        
            collidingState = c.colliding();
            if (collidingState == collidingState_Colliding)
            {
                c.doCollision(kCoefficientOfRestitutionDef);
                hadCollisionInLoop = true;

                                // Tell the solver we had a collision
                hadDiscontinuity = true;
            }
            else if (collidingState == collidingState_RestingContact)
            {
                // FIXME: deal with resting contact intelligently
                //c.doCollision(kCoefficientOfRestitutionDef);

                // Tell the solver we had a collision
                //hadDiscontinuity = true;
            }
        }
    } while (hadCollisionInLoop == true);

    return hadDiscontinuity;
}

//
// For each body-body pair A-B, try to find a separating plane
// If no such plane is found, conact or penetration has occured
//
void DynamicsWorld::findAllContacts(bool &interPenetration)
{
    ESeparationState separationState;

    // clear the list of contacts
    deleteContacts();

    BodyListT::const_iterator ciA; // constant because not modifying list
    BodyListT::const_iterator ciB; // constant because not modifying list
    WitnessListT::const_iterator ciW; // constant because not modifying list

    // FIXME: we should keep old witnesses around incase a penetration occurs
    // and then only save the new witnesses to the old witness cache if we
    // have no penetrations (only contacts and separations)
    Witness newWitness;

    interPenetration = false;

    ciW = mWitnesses.begin();

    for (ciA = mBodies.begin(); ciA != mBodies.end(); ++ciA)
    {       
        // start ciB on the body after ciA
        ciB = ciA;
        for (++ciB; ciB != mBodies.end(); ++ciB)
        {
            RigidBody& rbA = *((*ciA).second);
            assert(&rbA != NULL);
                        
            RigidBody& rbB = *((*ciB).second);
            assert(&rbB != NULL);
                        
            assert(&rbA != &rbB);
                        
            if (rbA.massInv != 0 || rbB.massInv != 0)
            {
                Witness& witness = **ciW;
                assert(&witness != NULL);
                                
                // use a copy of the witness
                newWitness = witness;

                // This will find a separating plane if one exists.
                separationState = findSeparatingPlane(rbA, rbB, newWitness);
                switch (separationState)
                {
                    case separationState_Separation:
                        // If we have separation, no action is neccessary
                        break;

                    case separationState_Contact:
                        // We have contact between rbA and rbB
                        newWitness.createContacts(mContacts);
                        break;

                    case separationState_Penetration:
                        // TODO: implement accurate contact finding
                        // If penetration occurs, we need to stop checking for
                        // penetrations, and try to back up in time to find when
                        // the contact originally occured

                        // For now, we will simply treat penetrations as contacts
#ifdef AVOID_PENETRATION
                        assert(false);
#else
                        newWitness.createContacts(mContacts);
#endif
                        interPenetration = true;
                        break;

                    default:
                        assert(false);
                }
                                
                // copy the new witness back to the cache
                witness = newWitness;
            }

            // Go to the witness for the next body-body pair
            ciW++;
        }
    }
}

void DynamicsWorld::deleteBodies(void)
{
    BodyListT::const_iterator ci;
        
    for (ci = mBodies.begin(); ci != mBodies.end(); ++ci)
    {
        RigidBody *o = (*ci).second;
        assert(o != NULL);
        delete o;
    }

    mBodies.clear();
}

void DynamicsWorld::deleteWitnesses(void)
{
    WitnessListT::const_iterator ci;
        
    for (ci = mWitnesses.begin(); ci != mWitnesses.end(); ++ci)
    {
        Witness *o = *ci;
        assert(o != NULL);
        delete o;
    }

    mWitnesses.clear();
}

void DynamicsWorld::deleteContacts(void)
{
    ContactListT::const_iterator ci;
        
    for (ci = mContacts.begin(); ci != mContacts.end(); ++ci)
    {
        Contact *o = *ci;
        assert(o != NULL);
        delete o;
    }

    mContacts.clear();
}

//
// Find a witness (a separating plane) which proves that
// the two bodies are in a state of separation.
//
//  - As soon as separation is found, return with the updated witness
//  - As each check is done, if separation is not found, try to
//    improve on the witness by storing the plane that is
//    most separating (greatest distance) the two bodies.
//    
ESeparationState DynamicsWorld::findSeparatingPlane(RigidBody &rbA, RigidBody &rbB, Witness &witness)
{
    // Possible separating planes include:
    //  1. faces of A
    //      2. faces of B
    //      3. a plane that includes an edge from A,
    //              and is parallel to an edge from B
    //      (Note: checking for planes that includes an edge from B,
    //              and is parallel to an edge from A is not neccessary
    //              because such planes would be parallel to those from
    //              step 3 above.)
        
    ESeparationState separationState;

    // first, check the witness
    separationState = witness.checkSeparation(rbA, rbB);
    if (separationState == separationState_Separation)
        return separationState_Separation;
        
    separationState = findWitnessFromPrimaryFace(rbA, rbB, witness, kContactThreshold);
    if (separationState == separationState_Separation)
        return separationState_Separation;

    separationState = findWitnessFromPrimaryFace(rbB, rbA, witness, kContactThreshold);
    if (separationState == separationState_Separation)
        return separationState_Separation;
        
    // check for type 3 separation planes
    separationState = findWitnessFromPrimaryEdge(rbA, rbB, witness, kContactThreshold);
    if (separationState == separationState_Separation)
        return separationState_Separation;
        
    // If we couldn't find separation, but we could find contact,
    // then at least we don't have penetration
    if (witness.getState() == separationState_Contact)
        return separationState_Contact;

    // No witness found means that penetration has occured
    // The witness from the previous penetration-free state will be
    // used to determine contacts
#ifdef AVOID_PENETRATION
    assert(false);
#endif
        
    assert(witness.getState() == separationState_Penetration);
    return separationState_Penetration;
}

ESeparationState DynamicsWorld::findWitnessFromPrimaryFace(RigidBody &rbPri, RigidBody &rbSec, Witness &witness, double contactThreshold)
{
    FaceListT *faces = rbPri.getFaces();
    FaceListT::const_iterator fi; // constant because not modifying list
        
    double dist;

    for (fi = faces->begin(); fi != faces->end(); ++fi)
    {
        DynFace* face = *fi;
        assert(face != NULL);
                
        dist = rbPri.distPlanePoly(face, &rbSec);

        if (dist > contactThreshold)
        {
            // we have found a new witness for these bodies
            witness.establish(&rbPri, &rbSec, face, dist, contactThreshold);
            return separationState_Separation;
        }
        else if (witness.isBetterSeparation(dist)) 
        {
            witness.improveSeparation(&rbPri, &rbSec, face, dist, contactThreshold);
        }
    }
        
    // no separating witness found, so return the best we have found
    return witness.getState();
}

ESeparationState DynamicsWorld::findWitnessFromPrimaryEdge(RigidBody &rbPri, RigidBody &rbSec, Witness &witness, double contactThreshold)
{
    if (rbPri.noEdgeCollisions() || rbSec.noEdgeCollisions())
        return witness.getState();

    EdgeListT *edgesPri = rbPri.getEdges();
    EdgeListT *edgesSec = rbSec.getNonParallelEdges();
    EdgeListT::const_iterator epi; // constant because not modifying list
    EdgeListT::const_iterator esi;
        
    DynFreePlane plane;
    bool validPlane;
    double distPri;
    double distSec;

    for (epi = edgesPri->begin(); epi != edgesPri->end(); ++epi)
    {
        DynEdge* edgePri = *epi;
        assert(edgePri != NULL);
                
        // FIXME: we don't need to re-check an edge from rbSec that is
        //  parallel to an edge we have already checked
        for (esi = edgesSec->begin(); esi != edgesSec->end(); ++esi)
        {
            DynEdge* edgeSec = *esi;
            assert(edgeSec != NULL);

            // make a plane that contains edgePri and is parallel to edgeSec
            plane.setEdges(edgePri, edgeSec);
            validPlane = (plane.getNormal() != NULL);
            if (validPlane)
            {
                // flip the plane's normal if it is pointing towared rbPri
                hduVector3Dd *otherVertexPri = edgePri->getNonColinearVertex();
                distPri = rbPri.distPlanePoint(&plane, otherVertexPri);
                if (distPri > -0.0001)
                    plane.reverse();

                // make sure the plane doesn't intersect ourselves

                // FIXME: we only need to check one vertex from each of the
                // two faces adjacent to edgePri, not all the vertices in rbPri
                                
                plane.reverse();  // flip it over to check ourselves
                distPri = rbPri.distPlanePoly(&plane, &rbPri);
                plane.reverse();

                if (distPri > -0.0001) // FIXME: optimal error coefficient?
                {
                    distSec = rbPri.distPlanePoly(&plane, &rbSec);

                    if (distSec > contactThreshold)
                    {
                        // we have found a new witness for these bodies
                        witness.establish(&rbPri, &rbSec, plane, distSec, contactThreshold);
                        return separationState_Separation;
                    }
                    else if (witness.isBetterSeparation(distSec)) 
                    {
                        witness.improveSeparation(&rbPri, &rbSec, plane, distSec, contactThreshold);
                    }
                }
            }
        }
    }
        
    // no separating witness found, so return the best we have found
    return witness.getState();
}

void DynamicsWorld::activateMouseSpring(int x, int y)
{
    assert(mouseSpringBody == NULL);

    hduVector3Dd mousePos(0,0,0);

    UnProjectUtilities::GetMousePosition(x, y, mousePos);

    RigidBody contactFinder;
        
    contactFinder.setName("mouse");

    // the RigidBody constructor will delete the vertex
    contactFinder.addVertex(new hduVector3Dd(mousePos));

    ESeparationState separationState;
    BodyListT::const_iterator ci; // constant because not modifying list
    Witness witness;
    Witness cleanWitness;

    for (ci = mBodies.begin(); ci != mBodies.end(); ++ci)
    {       
        RigidBody& rbPri = *((*ci).second);
        assert(&rbPri != NULL);
                
        if (rbPri.massInv != 0)
        {
            witness = cleanWitness;
            separationState = findWitnessFromPrimaryFace(rbPri, contactFinder, witness, kMouseContactThreshold);
            if (fabs(witness.getDistance()) < kMouseContactThreshold) 
                break;
        }
    }
        
    if (fabs(witness.getDistance()) >= kMouseContactThreshold)
        return;
        
    // ContactListT mouseContacts;
    // witness.createContacts(mouseContacts);
        
    // assert(mouseContacts.size() == 1);

    // Contact &contact = **mouseContacts.begin();
        
    // FIXME: use the point we clicked on, p, not the center of the body;
    // mouseSpringBody = contact.a;
    mouseSpringBody = witness.getRbPri();
    mouseSpringPos = mouseSpringBody->x;
}

void DynamicsWorld::moveMouseSpring(int x, int y)
{
    if (!mouseSpringBody)
        return;
        
    hduVector3Dd mouseNear, mouseFar;
    UnProjectUtilities::GetMouseRay(x, y, mouseNear, mouseFar);
        
    hduVector3Dd bodyPos = mouseSpringBody->x;
    mouseSpringPos = UnProjectUtilities::GetLineIntersectPlaneZ(mouseNear, mouseFar, bodyPos[2]);
}

void DynamicsWorld::deactivateMouseSpring(void)
{
    if (mouseSpringBody)
    {
        mouseSpringBody = NULL;
    }
}

// get current position of body in simulation
const hduVector3Dd& DynamicsWorld::getBodyPosition(HLuint id) const
{
    BodyListT::const_iterator i = mBodies.find(id);
    assert(i != mBodies.end());
    const RigidBody& rb = *((*i).second);
    return rb.x;
}

// get current orientation of body in simulation
const hduQuaternion& DynamicsWorld::getBodyOrientation(HLuint id) const
{
    BodyListT::const_iterator i = mBodies.find(id);
    assert(i != mBodies.end());
    const RigidBody& rb = *((*i).second);
    return rb.q;
}


/*
  Function:       OnHapticDeviceButtonDown
  ---------------------------------------------------------------------------
  Event callback function called when haptic device switch state changes.
*/
void HLCALLBACK OnHapticDeviceButtonDown(HLenum event, HLuint object, 
                                         HLenum thread, HLcache *cache, 
                                         void *userdata)
{
    if (thread == HL_CLIENT_THREAD)
    {
        assert(event == HL_EVENT_1BUTTONDOWN);
        assert(object != HL_OBJECT_ANY);

        // in the client thread we switch the mode
        // to control the object we are touching
        DynamicsWorld* pWorld = static_cast<DynamicsWorld*>(userdata);

        pWorld->setHapticMode(DynamicsWorld::CONTROL_OBJECT);

        // set touched object to be object we control with haptic
        // device
        pWorld->setHapticControlObject(object);

        // get the the current position and orientation of haptic
        // device
        hduVector3Dd proxyPos;
        hduQuaternion proxyRot;
        hlCacheGetDoublev(cache, HL_PROXY_POSITION, proxyPos);
        hlCacheGetDoublev(cache, HL_PROXY_ROTATION, proxyRot);
        
        // get rotation of touched object
        hduQuaternion bodyRot = pWorld->getBodyOrientation(object);
        
        // set the offset from the current position/rotation of the
        // touched object to the current position/rotation of the haptic
        // device proxy.  We will use this when setting the updated proxy
        // position/rotation and the updated body position/rotation.  Rather
        // than couple the orientation/position of the body to the initial
        // position/orientation of the haptic device, want to couple at this
        // offset so that if you grab the object and don't move, there will be
        // no force/torque on the object from the haptic device.  Only want
        // to generate from a change in position/orientation.
        pWorld->setHapticControlObjectOffset(
            pWorld->getBodyPosition(object) - proxyPos,
            proxyRot.inverse()*bodyRot);
    }
    else
    {
        // if we are called in the collision
        // thread we just disable proxy resolution
        // Do this in the collision thread to avoid a
        // a force discontinuity due to the lag between
        // contact with the shape and the reporting of the event
        // in the client thread.
        hlDisable(HL_PROXY_RESOLUTION);
    }
}

/*
  Function:       OnHapticDeviceButtonUp
  ---------------------------------------------------------------------------
  Event callback function called when haptic device switch state changes.
*/
void HLCALLBACK OnHapticDeviceButtonUp(HLenum event, HLuint object, 
                                       HLenum thread, HLcache *cache, 
                                       void *userdata)
{
    DynamicsWorld* pWorld = static_cast<DynamicsWorld*>(userdata);
    assert(pWorld);
    assert(event == HL_EVENT_1BUTTONUP);

    if (pWorld->getHapticMode() == DynamicsWorld::CONTROL_OBJECT &&
        pWorld->getHapticControlObject() != HL_OBJECT_ANY)
    {
        pWorld->setHapticMode(DynamicsWorld::TOUCH_OBJECTS);
        pWorld->setHapticControlObject(HL_OBJECT_ANY);
        hlEnable(HL_PROXY_RESOLUTION);
    }
}
