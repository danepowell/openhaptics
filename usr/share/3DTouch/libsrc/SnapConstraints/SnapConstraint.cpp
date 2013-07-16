/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

Description: 

  Base SnapConstraint class which can be used for generically applying a
  constraint to an input test position. 

******************************************************************************/

#include "SnapConstraintsAfx.h"
#include <SnapConstraints/SnapConstraint.h>

namespace SnapConstraints
{

double SnapConstraint::m_defaultSnapDistance = 2.5;

/******************************************************************************
 This function is meant as a wrapper for testConstraint. It controls setting 
 the done state for the constraint as well as implements the anti-constraint 
 behavior.
******************************************************************************/
bool SnapConstraint::applyConstraint(const hduVector3Dd &testPt,
                                     hduVector3Dd &proxyPt)
{
    double snapDist = testConstraint(testPt, proxyPt);

    if (snapDist < getSnapDistance())
    {
        if (isAntiConstraint())
        {
            /* Pretend we didn't have a successful constraint. */
            proxyPt = testPt;
        }

        /* Had collision. */
        return true;
    }
    else
    {
        setIsDone(true);

        /* didn't have collision. */
        return false;
    }

}

/******************************************************************************
 Output operator for debugging.
******************************************************************************/

// SnapConstraints << override: commented out by ARRAY, Array: dtc
//

/*
std::ostream &SnapConstraints::operator <<(std::ostream &os, 
                                      const SnapConstraint &constraint)
{    
    os << &constraint << " " << typeid(constraint).name();
    return os;
}
*/

} /* namespace SnapConstraints */

/*****************************************************************************/
