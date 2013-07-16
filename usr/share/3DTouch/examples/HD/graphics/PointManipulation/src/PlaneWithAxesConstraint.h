/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  PlaneWithAxesConstraint.h

Description:
  A composite snap constraint of a plane with two axial snaps and a point at
  the center.

*******************************************************************************/

#ifndef PlaneWithAxesConstraint_H_
#define PlaneWithAxesConstraint_H_

#include <SnapConstraints/CompositeConstraint.h>

class hduMatrix;

/******************************************************************************
 PlaneWithAxesConstraint
 Creates a plane constraint with snappable axes passing through the
 origin. The Z axis of the transform is used for the normal, and the X & Y
 axes are used for the snappable axes within the plane.
******************************************************************************/
class PlaneWithAxesConstraint : public SnapConstraints::CompositeConstraint
{
public:

    PlaneWithAxesConstraint(hduMatrix &xform, bool bAutoDelete = true);

};

#endif 

/******************************************************************************/

