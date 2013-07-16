/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  Constants.c

Description: 

  Defines constants of the simulation, e.g. stiffness , mass, and other 
  convenience consts.

*******************************************************************************/

#ifndef ContactModel_H_
#define ContactModel_H_

#include <HDU/hduVector.h>

/*******************************************************************************
  Simple case of contact between two spheres, which can be solved explicitly
  and simply.
*******************************************************************************/
class ContactModel
{
public:
    /* Contact model depends on relative position of two object,
       a fixed and a visitor. Constructor initializes
       center positions, and it assumes that initially there is no contact.
       It also initializes radius for both spheres. */
    ContactModel( double fixedRadius,
                  const hduVector3Dd fixed,
                  double visitorRadius,
                  const hduVector3Dd visitor);

    /* Updates the end effector position, and sets the new
       object position, and the relative dispacement of the two. */
    void UpdateEffectorPosition(const hduVector3Dd visitor);

    /* Given the current spatial (i.e. contact) configuration, calculate 
       the force on the visitor. */
    hduVector3Dd GetCurrentForceOnVisitor(void);
        
    /* This is the center of the object, after it has interacted
       with the contact model.
       If the visitor does not touch the fixed sphere, this is
       just the effector position. However, if the visitor touches the
       fixed sphere, the new contact point is basically the projection of 
       the effector position on the fixed sphere's surface. */
    hduVector3Dd GetCurrentContactPoint(void);

protected:
    hduVector3Dd m_fixedCenter;
    hduVector3Dd m_effectorPosition;
    hduVector3Dd m_visitorPosition;

    hduVector3Dd m_centerToEffector;
    hduVector3Dd m_visitorToCenter;

    /* Force on the effector. */
    hduVector3Dd m_forceOnVisitor;
    double m_currentDistance;

    /* The effective position where contact is established.
       Equals the sum of the radii of the two spheres. */
    double m_armsLength;
};

#endif /* ContactModel_H_ */

/******************************************************************************/
