/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
                                          
 File: ActorDynamics.cpp

 Created: 3/10/2004

 Desription:

 Defines a class that encapsulates dynamical properties of an actor 
 (e.g. master, slave particles). Performs state update, given a force.

******************************************************************************/


#include "ActorDynamics.h"

/******************************************************************************
    Constructor. Set history based on initial position,
    i.e. initial velocity and acceleration are zero
******************************************************************************/
ActorDynamics::ActorDynamics(const hduVector3Dd& initialPosition)
{
    m_mass = OBJECT_MASS;
    m_location[EVENBEFORE] = initialPosition;    
    m_location[BEFORE]     = initialPosition;
    m_location[NOW]        = initialPosition;
}    

/******************************************************************************
    Update history given new location
******************************************************************************/
void ActorDynamics::setNewPosition(const hduVector3Dd& position)
{
    m_location[EVENBEFORE] = m_location[BEFORE];    
    m_location[BEFORE]     = m_location[NOW];
    m_location[NOW]        = position;            
}

/******************************************************************************
    Get current position
******************************************************************************/
hduVector3Dd ActorDynamics::getPosition()
{
    return m_location[NOW];        
}

/******************************************************************************
    Get current velocity
******************************************************************************/
hduVector3Dd ActorDynamics::getVelocity()
{
    hduVector3Dd velocity;
    velocity = m_location[NOW] - m_location[BEFORE];
    return velocity;
}

/******************************************************************************
    Get current acceleration
******************************************************************************/
hduVector3Dd ActorDynamics::getAcceleration()
{
    hduVector3Dd accel;
    accel = m_location[NOW] - 2.0*m_location[BEFORE] + m_location[EVENBEFORE];
    return accel;
}

/******************************************************************************
    apply a force, and update the position
******************************************************************************/
void ActorDynamics::doDynamics(const hduVector3Dd& appliedForce)
{
    hduVector3Dd newPosition;

    newPosition = m_location[NOW] + (m_location[NOW] - m_location[BEFORE]) + 
                  appliedForce/m_mass;

    setNewPosition(newPosition);
}

/*****************************************************************************/
