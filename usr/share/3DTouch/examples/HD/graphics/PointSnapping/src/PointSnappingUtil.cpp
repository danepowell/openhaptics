/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  PointSnappingUtil.h

Description:

 Utility that uses the viewing transforms to determine a scale factor 
 that will allow us to specify the size of objects on the screen in pixel 
 dimensions.

*******************************************************************************/

#include "PointSnappingAfx.h"

#include <cassert>

#include "PointSnappingUtil.h"

/*******************************************************************************
 Given how big is a pixel in world coordinates
*******************************************************************************/
double computePixelToWorldScale(const GLdouble *modelview,
                                const GLdouble *projection,
                                const GLint *viewport)
{
    hduVector3Dd p0, p1;
    int bNoError;

    bNoError = gluUnProject(
        0, 0, 0,
        modelview,
        projection,
        viewport,
        &p0[0], &p0[1], &p0[2]);
    assert(bNoError);

    bNoError = gluUnProject(
        1, 0, 0,
        modelview,
        projection,
        viewport,
        &p1[0], &p1[1], &p1[2]);
    assert(bNoError);

    double pixelTworld = (p1 - p0).magnitude();
    return pixelTworld;
}

/******************************************************************************/
