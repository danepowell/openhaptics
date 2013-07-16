/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  PointManager.h

Description:

  Declares the pure virtual interface for PointManager.

*******************************************************************************/

#ifndef PointManager_H_
#define PointManager_H_

class IViewManager;

/******************************************************************************
 IPointManager Interface
******************************************************************************/
class IPointManager
{
public:
    static IPointManager *create();
    static void destroy(IPointManager *&pInterface);
    
    virtual void setup(IViewManager *pViewManager) = 0;
    virtual void cleanup() = 0;

    virtual int getNumPoints() const = 0;

    virtual const hduVector3Dd &getPointPosition(int nIndex) const = 0;
    virtual hduVector3Dd &getPointPosition(int nIndex) = 0;
    virtual void setPointPosition(int nIndex, const hduVector3Dd &position) = 0;

    virtual void drawPoints() = 0;
    virtual void drawLines() = 0;

    virtual void setPointHighlighted(int nIndex, bool bHighlighted) = 0;
    virtual bool isPointHighlighted(int nIndex) const = 0;

    virtual void setPointSelected(int nIndex, bool bSelected) = 0;    
    virtual bool isPointSelected(int nIndex) const = 0;

    virtual int pickPoint(int winx, int winy) const = 0;

protected:
    IPointManager() {}
    virtual ~IPointManager() {}

}; 

#endif // PointManager_H_

/******************************************************************************/
