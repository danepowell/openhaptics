/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

    PlaneConstraint.cpp

Description: 

    Subclass of SnapConstraint that implements a plane constraint

******************************************************************************/

#include "SnapConstraintsAfx.h"
#include <SnapConstraints/PlaneConstraint.h>

namespace SnapConstraints
{

/******************************************************************************
 Calculates the constraint position based on the input test position.
******************************************************************************/
double PlaneConstraint::testConstraint(const hduVector3Dd &testPt,
                                       hduVector3Dd &proxyPt) const
{
    hduVector3Dd testVec(testPt - m_point);
    proxyPt = testPt - dotProduct(m_normal, testVec) * m_normal;

    hduVector3Dd toPlane(proxyPt - testPt);
    return magnitude(toPlane);
}

} /* namespace SnapConstraints */

/*****************************************************************************/
