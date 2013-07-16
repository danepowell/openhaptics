/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  ForceModel.h

Description:

  Defines the equation of the force field.

*******************************************************************************/

#ifndef ForceModel_H_
#define ForceModel_H_

#include <HDU/hduVector.h>

class ForceModel
{
public:
    /* Force model depends on relative position of two points, a master and a 
       slave.  The master point is directly controlled by the device effector, 
       hence the name.  The slave point follows the master, using a dynamical 
       model of force. */
    ForceModel(const hduVector3Dd& master, const hduVector3Dd slave);

    /* Sets spatial aspects of the model. */
    void UpdatePosition(const hduVector3Dd& master, 
                        const hduVector3Dd& slave);

    /* Given the current spatial configuration, calculate the force.  Attractive 
       force if current distance bigger than arms length distance, otherwise 
       repulsive. */
    hduVector3Dd GetForceOnMaster();
                
    /* Force opposite to force on master. */
    hduVector3Dd GetForceOnSlave();

protected:
    hduVector3Dd m_masterLocation;
    hduVector3Dd m_slaveLocation;
    hduVector3Dd m_displacement;
    double m_currentDistance;

    /* The position where no force is felt. */
    double m_armsLength;    
};

#endif /* ForceModel_H_ */

/*****************************************************************************/

