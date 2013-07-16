/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  UnProjectUtilities.cpp

Description:

  Utility class for going between 2D screen space (mouse positions) and 3D.

*******************************************************************************/

#include "UnProjectUtilities.h"

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

void UnProjectUtilities::GetMouseRay(int x, int y, 
                                     hduVector3Dd &mouseNear, hduVector3Dd &mouseFar)
{
    GLint viewport[4];
    GLdouble mvMatrix[16], projMatrix[16];
    GLint realY; // OpenGL y coordinate position
    GLdouble wx, wy, wz; // returned world x, y, z coords
        
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

    // note viewport[3] is height of window in pixels
    realY = viewport[3] - (GLint) y - 1;

    gluUnProject(x, realY, 0.0, mvMatrix, projMatrix, viewport, &wx, &wy, &wz);
    mouseNear[0] = static_cast<double>(wx);
    mouseNear[1] = static_cast<double>(wy);
    mouseNear[2] = static_cast<double>(wz);
    gluUnProject(x, realY, 1.0, mvMatrix, projMatrix, viewport, &wx, &wy, &wz);
    mouseFar[0] = static_cast<double>(wx);
    mouseFar[1] = static_cast<double>(wy);
    mouseFar[2] = static_cast<double>(wz);
}

void UnProjectUtilities::GetMousePosition(int x, int y, hduVector3Dd &mousePos)
{
    GLfloat mouseZ;
    GLint viewport[4];
    GLdouble mvMatrix[16], projMatrix[16];
    GLint realY; // OpenGL y coordinate position
    GLdouble wx, wy, wz; // returned world x, y, z coords

        
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

    // note viewport[3] is height of window in pixels
    realY = viewport[3] - (GLint) y - 1;

    glReadPixels(x, realY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &mouseZ);
    gluUnProject(x, realY, mouseZ, mvMatrix, projMatrix, viewport, &wx, &wy, &wz);
    mousePos[0] = static_cast<double>(wx);
    mousePos[1] = static_cast<double>(wy);
    mousePos[2] = static_cast<double>(wz);
}

double UnProjectUtilities::GetDistanceFromLine(const hduVector3Dd &point, const hduVector3Dd &x1, const hduVector3Dd &x2)
{
    return (x2 - x1).crossProduct(x1 - point).magnitude() / (x2 - x1).magnitude();
}

hduVector3Dd UnProjectUtilities::GetLineIntersectPlaneZ(const hduVector3Dd &x1, const hduVector3Dd &x2, const double z)
{
    double r = (z - x1[2]) / (x2[2] - x1[2]);
        
    hduVector3Dd intersection;

    intersection[0] = x1[0] + r * (x2[0] - x1[0]);
    intersection[1] = x1[1] + r * (x2[1] - x1[1]);
    intersection[2] = z;

    return intersection;
}

/******************************************************************************/
