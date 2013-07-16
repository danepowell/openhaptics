/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  PointManipulationUtil.h

Description: 

  Utility routines for the PointManipulation example

*******************************************************************************/

#ifndef PointManipulationUtil_H_
#define PointManipulationUtil_H_


/*******************************************************************************
 Uses the viewing transforms to determine a scale factor that will allow us
 to specify the size of objects on the screen in pixel dimensions.
*******************************************************************************/
double computePixelToWorldScale(const GLdouble *modelview,
                                const GLdouble *projection,
                                const GLint *viewport);

#endif /* PointManipulationUtil_H_ */

/******************************************************************************/
