/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  PointConstraint.h

Description: 

  Subclass of SnapConstraint that implements a point constraint.

******************************************************************************/

#ifndef PointConstraint_H_
#define PointConstraint_H_

#include "SnapConstraint.h"

namespace SnapConstraints
{

/*****************************************************************************
 Class: PointConstraint

 Description: A generic point constraint.
******************************************************************************/
class PointConstraint : public SnapConstraint
{
public:
    PointConstraint(const hduVector3Dd &point = hduVector3Dd(),
                    bool bAutoDelete = true) :
        SnapConstraint(bAutoDelete),        
        m_point(point)        
    {
    }

    /* Calculates the SCP constraint position based on the input test 
       position. */
    virtual double testConstraint(const hduVector3Dd &testPt, 
                                  hduVector3Dd &proxyPt) const;

    void setPoint(const hduVector3Dd &point) { m_point = point; }
    hduVector3Dd const &getPoint() const { return m_point; }     

private:
    /* The location of the point constraint. */
    hduVector3Dd m_point;

};

} /* namespace SnapConstraints */

#endif /* PointConstraint_H_ */

/*****************************************************************************/
