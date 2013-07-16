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

#ifndef ViewManager_H_
#define ViewManager_H_

#include <HDU/hduVector.h>
#include <HDU/hduMatrix.h>

class IViewManager
{
public:

    enum ModifyViewType
    {
        MODIFY_VIEW_ROTATE,
        MODIFY_VIEW_SCALE,
        INVALID_MODIFY_VIEW,
    };

    static IViewManager *create();
    static void destroy(IViewManager *&pInterface);

    virtual void setup() = 0;
    virtual void cleanup() = 0;

    virtual void reshapeView(int width, int height) = 0;
    virtual void updateView() = 0;

    virtual int getWindowWidth() const = 0;
    virtual int getWindowHeight() const = 0;

    virtual bool isModifyingView() const = 0;
    virtual void startModifyView(ModifyViewType type, int x, int y) = 0;
    virtual void stopModifyView() = 0;
    virtual void modifyView(int x, int y) = 0;

    virtual bool toScreen(const hduVector3Dd &obj, hduVector3Dd &win) const = 0;
    virtual bool fromScreen(const hduVector3Dd &win, hduVector3Dd &obj) const = 0;

    virtual double getWindowToWorldScale() const = 0;

    virtual const hduMatrix &getViewTransform() const = 0;
    virtual const hduMatrix &getProjectionTransform() const = 0; 
    
protected:

    IViewManager() {}
    virtual ~IViewManager() {}

};

#endif // ViewManager_H_

/******************************************************************************/
