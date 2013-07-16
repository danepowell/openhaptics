/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  CompositeConstraint.h

Description: 

  Implements a SnapConstraint that has logic for resolving the constrained
  location by processing the test point through multiple SnapConstraints.
  Use this class to implement more complicated constraints, like a plane
  with axes and detents, or multiple axes that intersect with a point, etc.

******************************************************************************/

#ifndef CompositeConstraint_H_
#define CompositeConstraint_H_

#include "SnapConstraint.h"

namespace SnapConstraints
{

struct ConstraintHolder;

/******************************************************************************
 Class: CompositeConstraint

 Description: A generic composite constraint.
******************************************************************************/
class CompositeConstraint : public SnapConstraint
{
public:
    enum CompositeMode
    {
        SERIES, /* Passes the modified testPt to each constraint in series. */
        PARALLEL, /* Finds the closest constraint to the input testPt. */
    };

    CompositeConstraint(CompositeMode mode = PARALLEL, 
                       bool bAutoDelete = true);
    virtual ~CompositeConstraint();

    /* Calculates the constrained position based on the input test 
       position. */
    virtual double testConstraint(const hduVector3Dd &testPt,
                              hduVector3Dd &proxyPt) const;

    /* Choose between the two fundamental modes of applying a 
       group of constraints. */
    void setMode(CompositeMode mode) { m_mode = mode; }

    CompositeMode getMode() const { return m_mode; }

    ConstraintHolder *getConstraintHolder() { return m_pConstraintHolder; }

    const ConstraintHolder *getConstraintHolder() const
                                        { return m_pConstraintHolder; }

private:
    /* Calculates the constrained position based on the input test 
       position. */
    double testConstraintSeries(const hduVector3Dd &testPt,
                                hduVector3Dd &proxyPt) const;

    /* Calculates the constrained position based on the input test position. */
    double testConstraintParallel(const hduVector3Dd &testPt, 
                                hduVector3Dd &proxyPt) const;

    ConstraintHolder *m_pConstraintHolder;
    CompositeMode m_mode;
};

} /* namespace SnapConstraints */

#endif /* CompositeConstraint_H_ */

/*****************************************************************************/
