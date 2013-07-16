/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Contact.h

Description:

  Contact between two rigid bodies.

*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#include "Contact.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Contact::Contact() :
    a(0),
    b(0),
    p(0,0,0),
    n(0,0,0)
{
}

Contact::~Contact()
{
}

// Return true if bodies are in colliding contact.  The
// parameter 'kCollidingThreshold' is a small numerical tolerance
// used for deciding if bodies are colliding.
ECollidingState Contact::colliding(void)
{
    hduVector3Dd    padot = a->pointVelocity(p);
    hduVector3Dd    pbdot = b->pointVelocity(p);
    double          vrel = n.dotProduct(padot - pbdot);

    if (vrel > kCollidingThreshold)         // moving apart
        return collidingState_Separating;
    if (vrel > -kCollidingThreshold)        // resting contact
        return collidingState_RestingContact;
    else // vrel < -kCollidingThreshold
        return collidingState_Colliding;
}

// Compute and apply the impulse for the collision.
void Contact::doCollision(double coefficientOfRestitution)
{
    hduVector3Dd    padot = a->pointVelocity(p);
    hduVector3Dd    pbdot = b->pointVelocity(p);
    hduVector3Dd    ra = p - a->x;
    hduVector3Dd    rb = p - b->x;
    double          vrel = n.dotProduct(padot - pbdot);
    double          numerator = -(1 + coefficientOfRestitution) * vrel;
        
    // We'll calculate the denominator in four parts.
    double          term1 = a->massInv;
    double          term2 = b->massInv;
    double          term3 = n.dotProduct((a->Iinv * ra.crossProduct(n)).crossProduct(ra));
    double          term4 = n.dotProduct((b->Iinv * rb.crossProduct(n)).crossProduct(rb));
        
    // Compute the impulse magnitude.
    double          j = numerator / (term1 + term2 + term3 + term4);
    hduVector3Dd    force = j * n;

    // Apply the impulse to the bodies.
    a->P += force;
    b->P -= force;
    a->L += ra.crossProduct(force);
    b->L -= rb.crossProduct(force);

    // Recompute auxiliary variables.
    a->v = a->P * a->massInv;
    b->v = b->P * b->massInv;

    a->omega = a->Iinv * a->L;
    b->omega = b->Iinv * b->L;
}

/******************************************************************************/
