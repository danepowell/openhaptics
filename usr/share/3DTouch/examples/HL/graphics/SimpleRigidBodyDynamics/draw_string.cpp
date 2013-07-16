/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  draw_string.cpp

Description:

  Simple function for drawing strings using glutBitmapCharacter.

*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#include <stdarg.h>
#include <string.h>

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

#include "draw_string.h"

/*
     Function:       DrawBitmapString
     Usage:          DrawBitmapString(x, y, GLUT_BITMAP_8_BY_13, "Test")
     ---------------------------------------------------------------------------
     Draws a string using a bitmap font.  See GLUT documentation for the names
     of fonts.
*/
void DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...)
{
    int len, i;
    va_list args;
    char string[256];

        // special C stuff to interpret a dynamic set of arguments specified by "..."
    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    glRasterPos2f(x, y);
    len = (int) strlen(string);
        
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

/******************************************************************************/
