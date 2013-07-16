/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  PlaneConstraint.h

Description: 

  Subclass of SnapConstraint that implements a plane constraint.

******************************************************************************/

#ifndef PlaneConstraint_H_
#define PlaneConstraint_H_

#include "SnapConstraint.h"

namespace SnapConstraints
{

/*****************************************************************************
 Class: PlaneConstraint

 Description: A generic plane constraint.
******************************************************************************/
class PlaneConstraint : public SnapConstraint
{
public:
    PlaneConstraint(const hduVector3Dd &point = hduVector3Dd(),
                    const hduVector3Dd &normal = hduVector3Dd(),
                    bool bAutoDelete = true) :
        SnapConstraint(bAutoDelete),        
        m_point(point)
    {
        m_normal = normalize(normal);
    }

    /* Calculates the SCP constraint position based on the input 
       test position. */
    virtual double testConstraint(const hduVector3Dd &testPt, 
                                  hduVector3Dd &proxyPt) const;

    void setPoint(const hduVector3Dd &point) { m_point = point; }
    hduVector3Dd const &getPoint() const { return m_point; }     

    void setNormal(const hduVector3Dd &normal) { m_normal = normal; }
    hduVector3Dd const &getNormal() const { return m_normal; }     

private:
    /* Plane is defined as (x - p) * n = 0 */
    hduVector3Dd m_point;
    hduVector3Dd m_normal;

};

} /* namespace SnapConstraints */

#endif /* PlaneConstraint_H_ */

/*****************************************************************************/
