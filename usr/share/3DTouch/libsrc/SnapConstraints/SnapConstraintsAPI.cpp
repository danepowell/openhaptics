/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    SnapConstraintsAPI.cpp

Description: 

    This is an interface for managing SnapConstraint objects. A
    SnapConstraint is useful for constraining the haptic device motion and
    providing guidance to the user while performing a task.

******************************************************************************/

#include "SnapConstraintsAfx.h"
#include <SnapConstraints/ISnapConstraintsAPI.h>
#include <SnapConstraints/SnapConstraint.h>

#include <HDU/hduVector.h>

namespace SnapConstraints
{

namespace
{

class SnapConstraintsAPI : public ISnapConstraintsAPI
{
public:
    SnapConstraintsAPI() : m_pConstraint(0)
    {
    }

    virtual ~SnapConstraintsAPI()
    {
    }

    /* Call this method every servoloop tick to update the constrained 
       position of the device. This facility does not actually render a force. 
       It is the responsibility of the caller to then use the constrained proxy 
       location to render a force based on the current device position. */
    virtual bool updateConstraint(const hduVector3Dd &devicePt);

    /* This is the current constrained proxy location in device coordinates. */
    virtual const hduVector3Dd &getConstrainedProxy() const { return m_proxyPt; }


    /* Takes a pointer to the SnapConstraint object.  A SnapConstraint
       can either persist or be autodeleted when cleared.  The library
       will immediately begin rendering this constraint. */
    virtual void setConstraint(SnapConstraint *pConstraint);

    /* Returns a pointer to the constraint currently being applied. */
    virtual SnapConstraint *getConstraint() const { return m_pConstraint; }

    /* Clears the current applied constraint. */
    virtual void clearConstraint();

protected:
    SnapConstraint *m_pConstraint;
    hduVector3Dd m_proxyPt;

};

} /* anonymous namespace */

ISnapConstraintsAPI *ISnapConstraintsAPI::create() 
{
    return new SnapConstraintsAPI;
}

void ISnapConstraintsAPI::destroy(ISnapConstraintsAPI *&pInterface)
{
    if (pInterface)
    {
        SnapConstraintsAPI *pImp = static_cast<SnapConstraintsAPI *>(pInterface);
        delete pImp;
        pInterface = 0;
    }
}

/******************************************************************************
 Call this method every servoloop tick to update the constrained position of 
 the device. This facility does not actually render a force. It is the 
 responsibility of the caller to then use the constrained proxy location to 
 render a force based on the current device position.
******************************************************************************/
bool SnapConstraintsAPI::updateConstraint(const hduVector3Dd &devicePt)
{
    bool bConstrained = false;

    if (m_pConstraint)
    {
        bConstrained = m_pConstraint->applyConstraint(devicePt, m_proxyPt);
        
        if (m_pConstraint->isDone())
        {
            clearConstraint();
        }
    }
    
    if (!bConstrained)
    {
        m_proxyPt = devicePt;
    }    

    return bConstrained;
}


/******************************************************************************
 Takes a pointer to the SnapConstraint object. A SnapConstraint can either 
 persist or be autodeleted when cleared. The library will immediately begin 
 rendering this constraint.
******************************************************************************/
void SnapConstraintsAPI::setConstraint(SnapConstraint *pConstraint)
{
    if (m_pConstraint != pConstraint)
    {
        clearConstraint();
    }

    m_pConstraint = pConstraint;

    if (m_pConstraint)
    {
        m_pConstraint->setIsDone(false);
        m_pConstraint->onStartConstraint();
    }
}


/******************************************************************************
 Clears the current applied constraint.
******************************************************************************/
void SnapConstraintsAPI::clearConstraint()
{
    if (m_pConstraint)
    {
        m_pConstraint->onEndConstraint();

        if (m_pConstraint->isAutoDelete())
        {
            delete m_pConstraint;
        }   

        m_pConstraint = 0;
    }
}

} /* namespace SnapConstraints */

/*****************************************************************************/
