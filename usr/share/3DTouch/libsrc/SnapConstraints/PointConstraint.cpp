/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  PointConstraint.cpp

Description: 

  Subclass of SnapConstraint that implements a point constraint.

******************************************************************************/

#include "SnapConstraintsAfx.h"
#include <SnapConstraints/PointConstraint.h>

namespace SnapConstraints
{

/******************************************************************************
 Calculates the constraint position based on the input test position
******************************************************************************/
double PointConstraint::testConstraint(const hduVector3Dd &testPt,
                                       hduVector3Dd &proxyPt) const
{
    proxyPt = m_point;
    
    hduVector3Dd toPoint(m_point - testPt);
    return magnitude(toPoint);
}

} /* namespace SnapConstraints */

/*****************************************************************************/
