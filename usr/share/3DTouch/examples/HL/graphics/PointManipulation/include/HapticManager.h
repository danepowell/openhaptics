/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  IHapticManager.h

Description:

  Declares the pure virtual interface for IHapticManager.

*******************************************************************************/

#ifndef HapticManager_H_
#define HapticManager_H_

class IPointManager;
class IViewManager;

class IHapticManager
{
public:

    static IHapticManager *create();
    static void destroy(IHapticManager *&pInterface);

    virtual void setup(IPointManager *pPointManager, 
                       IViewManager *pViewManager) = 0;
    virtual void cleanup() = 0;

    virtual void updateWorkspace() = 0;

    virtual void drawCursor() = 0;
    virtual void feelPoints() = 0;

    virtual bool isManipulating() const = 0;

protected:

    IHapticManager() {}
    virtual ~IHapticManager() {}

};

#endif // HapticManager_H_

/******************************************************************************/
