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

#ifndef PointSnappingUtil_H_
#define PointSnappingUtil_H_

double computePixelToWorldScale(const GLdouble *modelview,
                                const GLdouble *projection,
                                const GLint *viewport);

#endif /* PointSnappingUtil_H_ */

/******************************************************************************/
