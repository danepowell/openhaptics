/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  PlaneWithAxesConstraint.cpp

Description:
  A composite snap constraint of a plane with two axial snaps and a point at
  the center.

*******************************************************************************/

#include "PointManipulationAfx.h"

#include "PlaneWithAxesConstraint.h"

#include <SnapConstraints/PointConstraint.h>
#include <SnapConstraints/LineConstraint.h>
#include <SnapConstraints/PlaneConstraint.h>
#include <SnapConstraints/ConstraintHolder.h>

#include <HDU/hduMatrix.h>

using namespace SnapConstraints;

/******************************************************************************
 Constructor
******************************************************************************/
PlaneWithAxesConstraint::PlaneWithAxesConstraint(hduMatrix &xform, 
                                                 bool bAutoDelete) :
    CompositeConstraint(CompositeConstraint::SERIES, bAutoDelete)
{
    /* First create the plane constraint. */
    hduVector3Dd planePoint;
    xform.multVecMatrix(hduVector3Dd(0, 0, 0), planePoint);
    
    hduVector3Dd planeNormal;
    xform.multDirMatrix(hduVector3Dd(0, 0, 1), planeNormal);    
    
    getConstraintHolder()->addConstraintBack(
        new PlaneConstraint(planePoint, planeNormal));

    /* Now we need to create the axes constraints, which will be two
       perpendicular snap lines in the plane with a snap point at the 
       center. */
    hduVector3Dd xAxisPoint;
    xform.multVecMatrix(hduVector3Dd(1, 0, 0), xAxisPoint);

    hduVector3Dd yAxisPoint;
    xform.multVecMatrix(hduVector3Dd(0, 1, 0), yAxisPoint);

    /* Create a new composite container for the two lines, since they both
       need to be tested in parallel (i.e. given equal opportunity). */
    CompositeConstraint *pLineComposite = 
        new CompositeConstraint(CompositeConstraint::PARALLEL);

    SnapConstraint *pLineConstraintX = new LineConstraint(planePoint, 
                                                          xAxisPoint);
    pLineConstraintX->setSnapDistance(pLineConstraintX->getSnapDistance()/2.0);
    pLineComposite->getConstraintHolder()->addConstraintBack(pLineConstraintX);

    SnapConstraint *pLineConstraintY = new LineConstraint(planePoint, 
                                                          yAxisPoint);
    pLineConstraintY->setSnapDistance(pLineConstraintY->getSnapDistance()/2.0);
    pLineComposite->getConstraintHolder()->addConstraintBack(pLineConstraintY);

    /* Add the line composite to the plane with axes composite. */
    getConstraintHolder()->addConstraintBack(pLineComposite);

    /* Finally add a point constraint to the plane with axes composite. */
    getConstraintHolder()->addConstraintBack(new PointConstraint(planePoint));           
}

    /******************************************************************************/
