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

#ifndef DynamicsSimulator_H_
#define DynamicsSimualtor_H_

#include "ActorDynamics.h"
#include "ForceModel.h"

class DynamicsSimulator 
{
public:
    DynamicsSimulator(const hduVector3Dd& master, 
                      const hduVector3Dd& slave) :
        m_master(master), 
        m_slave(slave), 
        m_forceModel(master, slave)
    {
    }
    
    /* Get the force on master object, for the current 
       positions of the slave and master. */
    hduVector3Dd GetForceOnMaster();

    /* Given the master's new position, calculate the force on the slave,
       apply it and calculate the slave's new position. */
    void Update(const hduVector3Dd& masterPosition);

    /* Public just for convenience. */
    ForceModel m_forceModel;
    ActorDynamics m_master;
    ActorDynamics m_slave;
};


#endif /* DynamicsSimulator_H_ */

/*****************************************************************************/
