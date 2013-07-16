/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module: 

  helper.h

Description:
        
  Utilities that set the graphics state.

*******************************************************************************/

#ifndef HelperHD_H_
#define HelperHD_H_

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>

void initGlut(int argc, char* argv[]);

void initGraphics(const hduVector3Dd &LLB, const hduVector3Dd &TRF);

/* Draws the cartesian axes. */
void drawAxes(double axisLength);

/* Draws a sphere to represent an electric charge. */
void drawSphere(GLUquadricObj* pQuadObj, 
                const hduVector3Dd &position,
                const float color[4],
                double sphereRadius);

/* Draws the force vector. */
void drawForceVector(GLUquadricObj* pQuadObj,
                     const hduVector3Dd &position,
                     const hduVector3Dd &forceVector,
                     double arrowThickness);
void setupGraphicsState();

#endif /* HelperHD_H_ */

/******************************************************************************/
