/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  UnProjectUtilities.h

Description:

  Utility class for going between 2D screen space (mouse positions) and 3D.

*******************************************************************************/

#ifndef UnProjectUtilities_H_
#define UnProjectUtilities_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicsMath.h"

class UnProjectUtilities  
{
public:
    static void GetMousePosition(int x, int y, hduVector3Dd &mousePos);
    static void GetMouseRay(int x, int y, 
                            hduVector3Dd &mouseNear, hduVector3Dd &mouseFar);
    static double GetDistanceFromLine(const hduVector3Dd &point, 
                                      const hduVector3Dd &x1, 
                                      const hduVector3Dd &x2);
    static hduVector3Dd GetLineIntersectPlaneZ(const hduVector3Dd &x1, 
                                               const hduVector3Dd &x2, 
                                               const double z);
};

#endif // UnProjectUtilities_H_

/******************************************************************************/
