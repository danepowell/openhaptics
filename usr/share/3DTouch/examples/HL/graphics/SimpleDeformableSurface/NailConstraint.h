/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  NailConstraint.h

Description:

  Implementation of particle system constraint to hold a particle in place.

*******************************************************************************/

#ifndef NailConstraint_H_
#define NailConstraint_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Constraint.h"

class NailConstraint : public Constraint
{
public:
    NailConstraint(int inParticle, hduVector3Dd inPosition);
    NailConstraint(int inParticle);
    virtual ~NailConstraint();
        
    virtual void ApplyConstraint(ParticleListT &particles); 
    virtual void Draw(ParticleListT &particles);
    virtual void FlexToSystem(ParticleListT &particles);

private:
    int mParticle;
    hduVector3Dd mPosition;
};

#endif // NailConstraint_H_

/*****************************************************************************/
