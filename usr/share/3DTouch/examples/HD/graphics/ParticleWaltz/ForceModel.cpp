/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.
  
OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.
    
For questions, comments or bug reports, go to forums at: 
http://dsc.sensable.com
      
Module Name: 
        
  ForceModel.cpp
          
Description:
          
  Defines the equation of the force field.
              
*******************************************************************************/

#include "Constants.h"
#include "ForceModel.h"

/*******************************************************************************
Constructor. Initially objects are defined to be in equilibrium,
i.e. initial configuration defines arms legth distance.
*******************************************************************************/
ForceModel::ForceModel(const hduVector3Dd& master, const hduVector3Dd slave)
{
    UpdatePosition(master, slave);
    
    /* Assume that we start at a neutral position. */
    m_armsLength = m_currentDistance; 
}

/*******************************************************************************
Updates positions of master and slave, displacement vector, and relative
distance.
*******************************************************************************/
void ForceModel::UpdatePosition(const hduVector3Dd& master, 
                                const hduVector3Dd& slave)
{
    m_currentDistance = 0.0;
    m_masterLocation = master;
    m_slaveLocation = slave;
    
    m_displacement = m_masterLocation - m_slaveLocation;
    m_currentDistance = m_displacement.magnitude();
    
}

/*******************************************************************************
Gets the force on the master particle, given the current displacement
*******************************************************************************/
hduVector3Dd ForceModel::GetForceOnMaster()
{
    double scale = -1.0*STIFFNESS*((m_currentDistance/m_armsLength)-1.0);
    
    hduVector3Dd force;
    force = m_displacement * scale;
    return force;
}

/*******************************************************************************
Gets the force on the slave particle, given the current displacement
*******************************************************************************/
hduVector3Dd ForceModel::GetForceOnSlave()
{
    hduVector3Dd force;
    force = -1.0*GetForceOnMaster();
    return force;
}

/******************************************************************************/
