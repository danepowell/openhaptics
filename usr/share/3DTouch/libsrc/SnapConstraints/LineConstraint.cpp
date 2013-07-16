/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  LineConstraint.cpp

Description: 

  Subclass of SnapConstraint that implements a line constraint.

******************************************************************************/

#include "SnapConstraintsAfx.h"
#include <SnapConstraints/LineConstraint.h>

namespace SnapConstraints
{
/******************************************************************************
 Calculates the constraint position based on the input test position
******************************************************************************/
double LineConstraint::testConstraint(const hduVector3Dd &testPt,
                                      hduVector3Dd &proxyPt) const
{
    hduVector3Dd lineVec(m_endPoint - m_startPoint);
    lineVec = normalize(lineVec);

    hduVector3Dd testVec(testPt - m_startPoint);

    /* Project the testPt onto the line. */
    proxyPt = m_startPoint + dotProduct(testVec, lineVec) * lineVec;

    /* Return the distance of the testPt from the line. */
    hduVector3Dd toLine(proxyPt - testPt);
    return magnitude(toLine);
}

} /* namespace SnapConstraints */

/*****************************************************************************/
