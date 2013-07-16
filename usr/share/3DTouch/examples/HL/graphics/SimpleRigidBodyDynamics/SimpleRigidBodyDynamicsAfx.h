/******************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this
software and use of this software is subject to the terms and conditions of the 
clickthrough Development License Agreement.

For questions, comments or bug reports, go to forums at: http://dsc.sensable.com
 
Module Name:

    SimpleRigidBodyDynamicsAfx.h

Description: 

    pre-compiled header file for SimpleRigidBodyDynamicsSample.

*******************************************************************************/

#pragma warning( disable : 4786 )  // identifier was truncated to '255' 
                                   // the "you are using STL" warning
#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/gl.h>
# include <GL/glu.h>
#elif defined(__APPLE__)
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <math.h>
