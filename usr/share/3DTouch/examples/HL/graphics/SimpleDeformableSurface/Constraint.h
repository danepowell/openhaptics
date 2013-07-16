/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Constraint.h

Description:

  Constraint interface for particle system.

*******************************************************************************/

#ifndef Constraint_H_
#define Constraint_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParticleSystem.h"

class Constraint
{
public:
    virtual void ApplyConstraint(ParticleListT &particles) = 0;
    virtual void Draw(ParticleListT &particles) = 0;
    virtual void FlexToSystem(ParticleListT &particles) = 0;
};

#endif // Constraint_H_

/******************************************************************************/
