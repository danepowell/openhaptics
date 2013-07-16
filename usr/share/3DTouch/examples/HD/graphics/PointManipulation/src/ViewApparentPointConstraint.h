/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ViewApparentPointConstraint.h

Description: 

  A composite SnapConstraint of a line and a point.

*******************************************************************************/

#ifndef ViewApparentPointConstraint_H_
#define ViewApparentPointConstraint_H_

#include <SnapConstraints/CompositeConstraint.h>
#include <SnapConstraints/PointConstraint.h>
#include <SnapConstraints/LineConstraint.h>

class ViewApparentPointConstraint : public SnapConstraints::CompositeConstraint
{
public:
    ViewApparentPointConstraint(bool bAutoDelete = true);
    virtual ~ViewApparentPointConstraint() {}

    SnapConstraints::PointConstraint * const getPointConstraint()
    { 
        return &m_pointConstraint;   
    }

    const SnapConstraints::PointConstraint * const getPointConstraint() const 
    {
        return &m_pointConstraint;
    }

    SnapConstraints::LineConstraint * const getLineConstraint()
    {
        return &m_lineConstraint;
    }

    const SnapConstraints::LineConstraint * const getLineConstraint() const 
    {
        return &m_lineConstraint;
    }

private:
    SnapConstraints::PointConstraint m_pointConstraint;
    SnapConstraints::LineConstraint m_lineConstraint;
}; /* ViewApparentPointConstraint */

#endif /* ViewApparentPointConstraint_H_ */

/******************************************************************************/
