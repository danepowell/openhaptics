/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticDeviceManager.h

Description:

  Declares the pure virtual interface for the IHapticDeviceManager.

*******************************************************************************/

#ifndef HapticDeviceManager_H_
#define HapticDeviceManager_H_

#include <HD/hd.h>

class IPointManager;

/*******************************************************************************
 IHapticDeviceManager
 
 This class.provides the application glue that marries haptic interactions with
 the application logic/state. The IHapticDeviceManager knows how to interact
 with the points maintained by the IPointManager instance. The 
 IHapticDeviceManager also handles mapping the workspace to world coordinates
 and displaying a 3D cursor for the device.
*******************************************************************************/
class IHapticDeviceManager
{
public:

    enum ManipulationStyle
    {
        FREE_MANIPULATION = 0,
        SPRING_MANIPULATION,
        FRICTION_MANIPULATION,
        PLANE_MANIPULATION,
        INVALID_MANIPULATION,
    };

    static IHapticDeviceManager *create();
    static void destroy(IHapticDeviceManager *&pInterface);

    virtual void setup(IPointManager *pPointManager) = 0;
    virtual void cleanup() = 0;

    virtual void updateState() = 0;
    virtual void updateWorkspace() = 0;

    virtual void drawCursor() = 0;

    virtual void setManipulationStyle(unsigned style) = 0;
    virtual unsigned getManipulationStyle() const = 0;

protected:
    IHapticDeviceManager() {}
    virtual ~IHapticDeviceManager() {}

}; 

#endif 

/******************************************************************************/
