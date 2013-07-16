/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  NailConstraint.cpp

Description:

  Implementation of particle system constraint to hold a particle in place.

*******************************************************************************/

#include "NailConstraint.h"

NailConstraint::NailConstraint(int inParticle, hduVector3Dd inPosition) :
    mParticle(inParticle),
    mPosition(inPosition)
{

}

NailConstraint::NailConstraint(int inParticle) :
    mParticle(inParticle),
    mPosition(0,0,0)
{

}

NailConstraint::~NailConstraint()
{

}

void NailConstraint::ApplyConstraint(ParticleListT &particles)
{
    Particle *p1 = particles[mParticle];
        
    p1->fixed = true;
    p1->f = hduVector3Dd(0,0,0);
}

void NailConstraint::Draw(ParticleListT &particles)
{
    float c[] = { 0, 1.0, 0 };
    glColor3fv(c);

    hduVector3Dd left(-0.25, 0.25, 2);
    hduVector3Dd right(0.25, 0.25, 2);

    Particle *p1 = particles[mParticle];

    glBegin(GL_LINES);
    hduVector3Dd v = p1->x + left;
    glVertex3dv(v);
    v = p1->x - left;
    glVertex3dv(v);
        
    v = p1->x + right;
    glVertex3dv(v);
    v = p1->x - right;
    glVertex3dv(v);
        
    glEnd();
}

void NailConstraint::FlexToSystem(ParticleListT &particles)
{
    Particle *p1 = particles[mParticle];

    mPosition = p1->x;
}

/******************************************************************************/
