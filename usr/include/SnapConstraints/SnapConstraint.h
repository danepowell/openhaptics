/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  SnapConstraint.h

Description: 

  Base SnapConstraint class which can be used for generically applying a
  constraint to an input test position. 

******************************************************************************/

#ifndef SnapConstraint_H_
#define SnapConstraint_H_

#include <HDU/hduVector.h>

#include <ostream>

namespace SnapConstraints
{

class SnapConstraint
{
public:
    /* Default Constuctor. */
    SnapConstraint(bool bAutoDelete = true) :
        m_snapDistance(m_defaultSnapDistance),
        m_bAutoDelete(bAutoDelete),
        m_bDone(false),
        m_bAntiConstraint(false),
        m_pUserData(0)
    {
    }

    /* Default Destructor. */
    virtual ~SnapConstraint()
    {
    }

    /* Calculates the constraint position based on the input test position. */
    virtual double testConstraint(const hduVector3Dd &testPt, 
                                  hduVector3Dd &proxyPt) const = 0;

    /* This function is meant as a wrapper for testConstraint and
       can provide some other non-geometry related logic for
       controlling when the constraint is done, etc. */
    virtual bool applyConstraint(const hduVector3Dd &testPt, 
                                 hduVector3Dd &proxyPt);

    /* This will get called by the SnapConstraintOwner when this constraint
       is made active. */
    virtual void onStartConstraint() {}

    /* This will get called by the SnapConstraintOwner when the constraint
       is no longer active. */
    virtual void onEndConstraint() {}

    /* The snap distance is static so that you can perform testConstraint
       and then test the return distance against the objects static
       snap distance. */
    void setSnapDistance(HDdouble distance) { m_snapDistance = distance; }
    HDdouble getSnapDistance() const { return m_snapDistance; }

    /* Gets the snap distance in world coords (will be affected by any scale
       between the phantom and the root). */
    static HDdouble getDefaultSnapDistance() { return m_defaultSnapDistance; }
    static void setDefaultSnapDistance(HDdouble dist) { 
        m_defaultSnapDistance = dist; }
            
    /* Indicates if the constraint should be deleted when cleared. */
    void setAutoDelete(bool bAutoDelete) { m_bAutoDelete = bAutoDelete; }
    bool isAutoDelete() const { return m_bAutoDelete; }

    /* Anticonstraint is a mechanism that allows a constraint to exist
       that doesn't actually constrain the device. */
    void setAntiConstraint(bool bEnable) { m_bAntiConstraint = bEnable; }
    bool isAntiConstraint() const { return m_bAntiConstraint; }

    /* Indicates that the constraint can be cleared. */
    void setIsDone(bool bDone) { m_bDone = bDone; }
    bool isDone() const { return m_bDone; }

    void setUserData(void *pUserData) { m_pUserData = pUserData; }
    void *getUserData() { return m_pUserData; }
    const void *getUserData() const { return m_pUserData; }

private:

    /* The snap distance used for this instance. */
    HDdouble m_snapDistance;

    /* A general purpose snap tolerance used for all constraints. */
    static HDdouble m_defaultSnapDistance;

    /* A flag that indicates if the constraint should be deleted when 
       cleared. */
    bool m_bAutoDelete;

    /* A flag that indicates if the constraint can be cleared. */
    bool m_bDone;

    /* Anticonstraint is a mechanism that allows a constraint to nullify
       another constraint based on priority. */
    bool m_bAntiConstraint;

    /* Allows the client to associate private data with the constraint which
       can be retrieved while the snap is being applied. */
    void *m_pUserData;

    /* Output operator for debugging. */
    friend std::ostream& operator <<(std::ostream &os, const SnapConstraint &constraint);

};

/* Output operator for debugging. */
std::ostream &operator <<(std::ostream& os, const SnapConstraint &constraint);

} /* namespace SnapConstraints */

#endif /* SnapConstraint_H_ */

/*****************************************************************************/
