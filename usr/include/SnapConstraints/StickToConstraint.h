/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  StickToConstraint.h

Description: 

  This is a special wrapper for a SnapConstraint that changes the behavior
  of the constraint so that the user can not pull off of it. Use this
  class to turn any SnapConstraint into a StickToConstraint.

******************************************************************************/

#ifndef StickToConstraint_H_
#define StickToConstraint_H_

#include "SnapConstraint.h"

namespace SnapConstraints
{

/*****************************************************************************
 Class: StickToConstraint

 Description: A generic stick-to constraint
******************************************************************************/
template <class T>
class StickToConstraint : public SnapConstraint
{
public:
    StickToConstraint(T *pConstraint = 0, bool bAutoDelete = true) :
        SnapConstraint(bAutoDelete),
        m_pConstraint(pConstraint)
    {
    }

    virtual ~StickToConstraint()
    {
        if (m_pConstraint && m_pConstraint->isAutoDelete())
        {
            delete m_pConstraint;
            m_pConstraint = 0;
        }
    }

    void setConstraint(T *pConstraint) { m_pConstraint = pConstraint; }
    T *getConstraint() { return m_pConstraint; }
    const T *getConstraint() const { return m_pConstraint; }

    /* Calculates the constraint position based on the input test position. */
    virtual double testConstraint(const hduVector3Dd &testPt, 
                                  hduVector3Dd &proxyPt) const
    {
        return m_pConstraint->testConstraint(testPt, proxyPt);
    }    

    virtual bool applyConstraint(const hduVector3Dd &testPt, 
                                 hduVector3Dd &proxyPt)
    {
        m_pConstraint->applyConstraint(testPt, proxyPt);
        setIsDone(false);
        return true;
    }

private:
    T *m_pConstraint;

};

} /* namespace SnapConstraints */

#endif /* StickToConstraint_H_ */

/*****************************************************************************/
