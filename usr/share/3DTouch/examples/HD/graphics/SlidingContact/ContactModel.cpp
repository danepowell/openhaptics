/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ContactModel.cpp

Description: 

  Defines the equation of the force field.

*******************************************************************************/

#include "Constants.h"
#include "ContactModel.h"

/*******************************************************************************
 Constructor. Force model depends on relative position of two objects,
 a fixed and a visitor. Constructor initializes center positions, and it 
 assumes that initially there is no contact. It also initializes radii
 for both spheres.
*******************************************************************************/

ContactModel::ContactModel(double fixedRadius,
                            const hduVector3Dd fixed,
                            double visitorRadius,
                            const hduVector3Dd visitor)
{
    m_fixedCenter = fixed;

    /* Intersection of two spheres is equivalent to intersection
       of a point and a sphere of effective radius (arms length) 
       equal to the sum of the two radii. */
    m_armsLength = fixedRadius + visitorRadius;
    UpdateEffectorPosition(visitor);
}

/*******************************************************************************
Updates the end effector position, and sets the new
object position, and the relative dispacement of the two.
*******************************************************************************/
void ContactModel::UpdateEffectorPosition(const hduVector3Dd visitor)
{

    m_currentDistance = 0.0;
    m_effectorPosition = visitor;
    m_centerToEffector = m_effectorPosition - m_fixedCenter;
    
    m_currentDistance =  m_centerToEffector.magnitude();
    if(m_currentDistance > m_armsLength)
    {
        m_visitorPosition = m_effectorPosition;
        m_forceOnVisitor.set(0.0, 0.0, 0.0);
    }
    else
    {
        /* if we are nearly at the center don't recalculate anything,
           since the center is a singular point. */
        if(m_currentDistance > EPSILON)
        {
            double scale = m_armsLength/m_currentDistance;

            /* project the effector point on the effective
               sphere (i.e. at arms length) surface. */
            m_visitorPosition = m_fixedCenter + scale*m_centerToEffector;
   
            /* force is calculated from F=kx; k is the stiffness
               and x is the vector magnitude and direction.  In this case,
               k = STIFFNESS, and x is the penetration vector from 
               the actual position to the desired position. */
            m_forceOnVisitor = STIFFNESS*(m_visitorPosition-m_effectorPosition);
        } 
    }
}

/*******************************************************************************
 Gets the force on the visitor particle, given the current displacement.
*******************************************************************************/
hduVector3Dd ContactModel::GetCurrentForceOnVisitor()
{
        return m_forceOnVisitor;
}

/*******************************************************************************
 Retrieve the current contact point, i.e. the center of the visitor
 sphere.
*******************************************************************************/
hduVector3Dd ContactModel::GetCurrentContactPoint()
{
        return m_visitorPosition;
}

/******************************************************************************/
