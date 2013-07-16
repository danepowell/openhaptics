/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  SpringConstraint.h

  Description:

  Implementation of particle constraint representing spring between two
  particles.
*******************************************************************************/

#ifndef SPRING_CONSTRAINT_H_
#define SPRING_CONSTRAINT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Constraint.h"

class SpringConstraint : public Constraint
{
public:
    SpringConstraint(int inParticle1, int inParticle2, double inLength);
    SpringConstraint(int inParticle1, int inParticle2);
    virtual ~SpringConstraint();

    virtual void ApplyConstraint(ParticleListT &particles);
    virtual void Draw(ParticleListT &particles);
    virtual void FlexToSystem(ParticleListT &particles);
        
    void SetParticle(int inParticle) { mParticle2 = inParticle; }
    bool ParticlesAreValid(void) 
    { 
        return (mParticle1 != mParticle2) && 
            (mParticle1 != -1) &&
            (mParticle2 != -1); 
    }
    
    static void SetSpringConstant(double inKS) { ks = inKS; }
    static void SetSpringDampingConstant(double inKD) { kd = inKD; }
    static double GetSpringConstant(void) { return ks; }
    static double GetSpringDampingConstant(void) { return kd; }

private:
    int mParticle1;
    int mParticle2;
    double mLength; // rest length
    bool mHasFixedLength;
    static double ks; // spring stiffness
    static double kd; // damping constant
};

#endif // SPRING_CONSTRAINT_H_

/******************************************************************************/

