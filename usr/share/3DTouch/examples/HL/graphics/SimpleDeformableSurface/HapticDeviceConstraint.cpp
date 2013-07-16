/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticDeviceConstraint.cpp

Description:

  Implementation of particle system constraint to let you use haptic device
  to pull on a particle.

*******************************************************************************/

#include "HapticDeviceConstraint.h"

HapticDeviceConstraint::HapticDeviceConstraint() :
    mParticle(0),
    mForce(0,0,0),
    mState(false)
{ 
}

HapticDeviceConstraint::~HapticDeviceConstraint()
{
}

void HapticDeviceConstraint::ApplyConstraint(ParticleListT &particles)
{
    if (!mState)    // don't apply if not active
        return;
        
    Particle *p1 = particles[mParticle];
        
    if (!p1->fixed)
        p1->f += mForce;
}

void HapticDeviceConstraint::Draw(ParticleListT &particles)
{
    if (!mState)    // don't draw if not active
        return;

    float c[] = { .5, 0, 1 };
    glColor3fv(c);

    Particle *p1 = particles[mParticle];

    glBegin(GL_LINES);
        
    glVertex3dv(p1->x);
    glVertex3dv(p1->x + mForce);
        
    glEnd();
}

/******************************************************************************/
