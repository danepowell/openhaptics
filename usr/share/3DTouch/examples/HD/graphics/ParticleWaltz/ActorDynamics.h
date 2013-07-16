/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  ActorDynamics.h

Description:

 Defines a class that encapsulates dynamical properties of an actor 
 (e.g. master, slave particles). Performs state update, given a force.

*******************************************************************************/

#ifndef ActorDynamics_H_
#define ActorDynamics_H_

#include <HDU/hduVector.h>

/* private defines. */
#define TIMESLICES 3
#define NOW 2
#define BEFORE 1
#define EVENBEFORE 0

#include "Constants.h"

class ActorDynamics
{
    
public:
    ActorDynamics(const hduVector3Dd& initialPosition);    
        
    void setNewPosition(const hduVector3Dd& position);

    /* get current position. */
    hduVector3Dd getPosition();

    /* get current velocity. */
    hduVector3Dd getVelocity();

    /* get current acceleration. */
    hduVector3Dd getAcceleration();

    /* apply a force, and update the position. */
    void doDynamics(const hduVector3Dd& appliedForce);

    /* get the object's mass. */
    double getMass(){return m_mass;}

protected:
    hduVector3Dd m_location[TIMESLICES];
    hduVector3Dd m_velocity[TIMESLICES];
    hduVector3Dd m_acceleration[TIMESLICES];
    double m_mass;
};

#endif

/*****************************************************************************/
