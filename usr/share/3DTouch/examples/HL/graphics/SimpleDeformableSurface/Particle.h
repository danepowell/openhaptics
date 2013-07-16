/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  Particle.h

Description:

  Particle with position, velocity, mass.

*******************************************************************************/

#ifndef Particle_H_
#define Particle_H_

#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/gl.h>
#elif defined(__APPLE__)
# include <OpenGL/gl.h>
#endif

#include <iostream>
#include "DynamicsMath.h"

class Particle
{
public:
    hduVector3Dd x;
    hduVector3Dd v;
    hduVector3Dd f;

    static double mass;
    bool fixed;
        
    Particle() :
        x(0, 0, 0),
        v(0, 0, 0),
        f(0, 0, 0),
        fixed(false)
    {
    }

    Particle(double inMass) :
        x(0, 0, 0),
        v(0, 0, 0),
        f(0, 0, 0),
        fixed(false)
    {
        mass = inMass;  // note: static, so affects all particles
    }

    static void BeginDraw(void);
    void Draw(void);
    static void EndDraw(void);

    static void SetMass(double inMass) { mass = inMass; }
};

#endif // Particle_H_

/*****************************************************************************/

