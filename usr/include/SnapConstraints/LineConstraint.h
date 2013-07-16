/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  LineConstraint.h

Description: 

  Subclass of SnapConstraint that implements a line constraint.

******************************************************************************/

#ifndef LineConstraint_H_
#define LineConstraint_H_

#include "SnapConstraint.h"

namespace SnapConstraints
{

/******************************************************************************
 Class: LineConstraint

 Description: A generic line constraint.
******************************************************************************/
class LineConstraint : public SnapConstraint
{
public:
    LineConstraint(const hduVector3Dd &startPoint = hduVector3Dd(),
                   const hduVector3Dd &endPoint = hduVector3Dd(),
                   bool bAutoDelete = true) :
        SnapConstraint(bAutoDelete),        
        m_startPoint(startPoint),
        m_endPoint(endPoint){}

    /* Calculates the SCP constraint position based on the input 
       test position. */
    virtual double testConstraint(const hduVector3Dd &testPt, 
                                  hduVector3Dd &proxyPt) const;

    void setStartPoint(const hduVector3Dd &point) {m_startPoint = point; }
    hduVector3Dd const &getStartPoint() const {return m_startPoint; }     

    void setEndPoint(const hduVector3Dd &point) {m_endPoint = point; }
    hduVector3Dd const &getEndPoint() const { return m_endPoint; }     

private:
    /* The line parametric is defined as p(t) = (e - s)*t + s; */
    hduVector3Dd m_startPoint;
    hduVector3Dd m_endPoint;

};

} /* namespace SnapConstraints */

#endif /* LineConstraint_H_ */

/*****************************************************************************/
