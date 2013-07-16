/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticDeviceConstraint.h

Description:

  Implementation of particle system constraint to let you use haptic device
  to pull on a particle.

*******************************************************************************/

#ifndef HapticDeviceConstraint_H_
#define HapticDeviceConstraint_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Constraint.h"

class HapticDeviceConstraint : public Constraint
{
public:

    HapticDeviceConstraint();
    virtual ~HapticDeviceConstraint();

    virtual void ApplyConstraint(ParticleListT &particles);
    virtual void Draw(ParticleListT &particles);

    virtual void FlexToSystem(ParticleListT &particles) { }

    void SetState(bool inState) { mState = inState; }
    void SetForce(const hduVector3Dd& inForce) { mForce = inForce; }
    void SetParticle(int inParticle) { mParticle = inParticle; }
        
    int GetParticle(void) { return mParticle; }

private:
    int mParticle;
    hduVector3Dd mForce;
    bool mState; // on or off
};

#endif // HapticDeviceConstraint_H_

/*****************************************************************************/
