/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ISnapConstraintsAPI.h

Description: 

  This is an interface for managing SnapConstraint objects. A
  SnapConstraint is useful for constraining the haptic device motion and
  providing guidance to the user while performing a task.

******************************************************************************/

#ifndef ISnapConstraintsAPI_H_
#define ISnapConstraintsAPI_H_

#include <HDU/hduVector.h>

namespace SnapConstraints
{

class SnapConstraint;

/******************************************************************************
 ISnapConstraintsAPI

 Use this facility to haptically render a constraint for the haptic device.
 Typically, the graphics loop checks if a constraint should be activated, 
 then sets it only when haptic rendering of it is needed. The constraint
 should be updated in the servo loop to determine the constrained location
 give the current device position. The constraint will automatically be
 cleared when the device snaps off of it.
******************************************************************************/
struct ISnapConstraintsAPI
{    
    static ISnapConstraintsAPI *create();
    static void destroy(ISnapConstraintsAPI *&pInterface);

    /* Call this method every servoloop tick to update the constrained 
       position of the device. This facility does not actually render a
       force. It is the responsibility of the caller to then use the 
       constrained proxy location to render a force based on the current
       device position. */
    virtual bool updateConstraint(const hduVector3Dd &devicePt) = 0;

    /* This is the current constrained proxy location in device coordinates. */
    virtual const hduVector3Dd &getConstrainedProxy() const = 0;

    /* Takes a pointer to the SnapConstraint object. A SnapConstraint
       can either persist or be autodeleted when cleared. */
    virtual void setConstraint(SnapConstraint *pConstraint) = 0;

    /* Returns a pointer to the constraint currently being applied. */
    virtual SnapConstraint *getConstraint() const = 0;

    /* Clears the current applied constraint. */
    virtual void clearConstraint() = 0;
};

} /* namespace SnapConstraints */

#endif /* ISnapConstraintsAPI_H_ */

/*****************************************************************************/
