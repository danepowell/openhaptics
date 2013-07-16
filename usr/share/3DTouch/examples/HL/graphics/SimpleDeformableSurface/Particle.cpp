/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Particle.cpp

Description:

  Particle with position, velocity, mass.

*******************************************************************************/

#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/gl.h>
#elif defined(__APPLE__)
# include <OpenGL/gl.h>
#endif

#include "Particle.h"

static const double kDefaultMass = 1;
double Particle::mass = kDefaultMass;

void Particle::BeginDraw(void)
{
    float c[] = { 1.0, 0, 0 };
    glColor3fv(c);
    glBegin(GL_POINTS);
}

void Particle::Draw(void)
{
    glVertex3dv(&x[0]);
}

void Particle::EndDraw(void)
{
    glEnd();
}

/******************************************************************************/
