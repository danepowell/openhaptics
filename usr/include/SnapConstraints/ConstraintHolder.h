/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ConstraintHolder.h

Description: 

  A basic list oriented container for SnapConstraints.

******************************************************************************/

#ifndef ConstraintHolder_H_
#define ConstraintHolder_H_

namespace SnapConstraints
{

class SnapConstraint;

/*****************************************************************************
 Struct: SnapConstraint

 Description: placeholder that one can add pointers to constraints 
              (of base class SnapConstraint)

******************************************************************************/
struct ConstraintHolder
{
    static ConstraintHolder *create();
    static void destroy(ConstraintHolder *&pInstance);

    virtual void addConstraintFront(SnapConstraint *pConstraint) = 0;
    virtual void addConstraintBack(SnapConstraint *pConstraint) = 0;
    virtual void removeConstraint(SnapConstraint *pConstraint) = 0;
    virtual void clearConstraints() = 0;

    /* Methods for iterating through constraints. */
    virtual void begin() = 0;
    virtual void next() = 0;
    virtual bool done() const = 0;
    virtual SnapConstraint *current() = 0;
    virtual const SnapConstraint *current() const = 0;
};

} /* namespace SnapConstraints */

#endif /* ConstraintHolder_H_ */

/*****************************************************************************/
