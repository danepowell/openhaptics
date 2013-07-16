/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynamicsSimulator.h

Description:

  Defines a class that puts together the force model with the master and
  slave actors. This is the fundamental datastructure that is synchronized 
  between the haptics and graphics threads.

*******************************************************************************/

#include "DynamicsSimulator.h"

/******************************************************************************
 Given the master's new position, calculate the force on the slave, apply it 
 and calculate the slave's new position.
******************************************************************************/
void DynamicsSimulator::Update(const hduVector3Dd& masterPosition)
{
    // The master has moved. Update its trajectory data.
    m_master.setNewPosition(masterPosition);


    // Slave has not moved yet.
    hduVector3Dd slavePosition = m_slave.getPosition();

    // Update the positions in the force model.
    m_forceModel.UpdatePosition(masterPosition, slavePosition);
        
    // Force on slave.
    hduVector3Dd forceOnSlave = m_forceModel.GetForceOnSlave(); 

    // Move slave to new position.
    m_slave.doDynamics(forceOnSlave);
}

/******************************************************************************
 Get the force on the master.
******************************************************************************/
hduVector3Dd DynamicsSimulator::GetForceOnMaster()
{
    return m_forceModel.GetForceOnMaster();
}

/*****************************************************************************/
