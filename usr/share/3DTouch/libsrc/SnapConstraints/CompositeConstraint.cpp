/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  CompositeConstraint.cpp

Description: 

  Implements a SnapConstraint that has logic for resolving the constrained
  location by processing the test point through multiple SnapConstraints.
  Use this class to implement more complicated constraints, like a plane
  with axes and detents, or multiple axes that intersect with a point, etc.

******************************************************************************/

#include "SnapConstraintsAfx.h"
#include <SnapConstraints/CompositeConstraint.h>
#include <SnapConstraints/ConstraintHolder.h>

#include <float.h>
#include <assert.h>

namespace SnapConstraints
{

/******************************************************************************
 Constructor
******************************************************************************/
CompositeConstraint::CompositeConstraint(CompositeMode mode, bool bAutoDelete) :
    SnapConstraint(bAutoDelete),
    m_mode(mode)
{
    m_pConstraintHolder = ConstraintHolder::create();
}

/******************************************************************************
 Destructor
******************************************************************************/
CompositeConstraint::~CompositeConstraint()
{
    ConstraintHolder::destroy(m_pConstraintHolder);
}

/******************************************************************************
 Calculates the constraint position based on the input test position
******************************************************************************/
double CompositeConstraint::testConstraint(const hduVector3Dd &testPt,
                                           hduVector3Dd &proxyPt) const
{
    assert(m_pConstraintHolder);
    
    if (m_mode == SERIES)
    {
        return testConstraintSeries(testPt, proxyPt);
    }
    else if (m_mode == PARALLEL)
    {
        return testConstraintParallel(testPt, proxyPt);   
    }
    else
    {
        assert(!"Unimplemented");
        return DBL_MAX;
    }
}

/******************************************************************************
 Calculates the constraint position based on the input test position
******************************************************************************/
double CompositeConstraint::testConstraintSeries(const hduVector3Dd &testPt,
                                                 hduVector3Dd &proxyPt) const
{
    hduVector3Dd tempTestPt = testPt;
    hduVector3Dd tempProxyPt;
    bool bFirstTest = true;
    
    /* Test the constraints in the order provided. */
    for (m_pConstraintHolder->begin();
         !m_pConstraintHolder->done();
         m_pConstraintHolder->next())
    {
        SnapConstraint *pConstraint = m_pConstraintHolder->current();
        if (pConstraint)
        {
            double dist = pConstraint->testConstraint(tempTestPt, tempProxyPt);
            
            if (bFirstTest || dist < pConstraint->getSnapDistance())
            {
                /* Note: We are replacing the testPt to be the newly constrained
                   position, which allows for constraints to be applied serially.
                   (i.e. constrain to plane, then constrain to line, then constrain
                   to point, etc.). */
                tempTestPt = tempProxyPt;
                proxyPt = tempProxyPt;
                bFirstTest = false;
            }            
        }
    }

    hduVector3Dd testToProxy(testPt - proxyPt);
    return magnitude(testToProxy);
}

/******************************************************************************
 Calculates the constraint position based on the input test position
******************************************************************************/
double CompositeConstraint::testConstraintParallel(const hduVector3Dd &testPt,
                                                   hduVector3Dd &proxyPt) const
{
    double minDist = DBL_MAX;
    hduVector3Dd tempProxyPt;
    
    /* Test the constraints in the order provided. */
    for (m_pConstraintHolder->begin();
         !m_pConstraintHolder->done();
         m_pConstraintHolder->next())
    {
        SnapConstraint *pConstraint = m_pConstraintHolder->current();
        if (pConstraint)
        {
            double dist = pConstraint->testConstraint(testPt, tempProxyPt);

            /* Check to see if this constraint is closer and of the same
               or greater priority. If so, it should take precedence. */
            if (dist < minDist)
            {
                minDist = dist;                
                proxyPt = tempProxyPt;
            }              
        }
    }

    hduVector3Dd testToProxy(testPt - proxyPt);
    return magnitude(testToProxy);
}

} /* namespace SnapConstraints */

/*****************************************************************************/
