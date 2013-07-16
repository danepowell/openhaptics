/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ViewApparentPointConstraint.cpp

Description:
  
  Creates a SnapConstraints composition for feeling a point along a line.
  The typical usage is for the line to be aligned with the view and passing
  through the point.
*******************************************************************************/

#include "PointSnappingAfx.h"

#include "ViewApparentPointConstraint.h"
#include <SnapConstraints/ConstraintHolder.h>

using namespace SnapConstraints;

/*******************************************************************************
 Constructor
*******************************************************************************/
ViewApparentPointConstraint::ViewApparentPointConstraint(bool bAutoDelete) :
    CompositeConstraint(CompositeConstraint::SERIES, bAutoDelete)
{
    /* Turn off autodelete for the subconstraints, since this composite
       constraint owns them. */

    m_lineConstraint.setAutoDelete(false);
    getConstraintHolder()->addConstraintBack(&m_lineConstraint);

    m_pointConstraint.setAutoDelete(false);
    getConstraintHolder()->addConstraintBack(&m_pointConstraint);    
}

/******************************************************************************/
