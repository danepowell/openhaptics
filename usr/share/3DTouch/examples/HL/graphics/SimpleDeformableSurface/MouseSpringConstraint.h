/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  MouseSpringConstraint.h

Description:

  Implementation of particle system constraint to let you use the mouse
  to pull on a particle.
*******************************************************************************/

#if !defined(AFX_MOUSESPRINGCONSTRAINT_H__A6A078ED_7494_45BF_8CF7_CD62301E3429__INCLUDED_)
#define AFX_MOUSESPRINGCONSTRAINT_H__A6A078ED_7494_45BF_8CF7_CD62301E3429__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Constraint.h"

class MouseSpringConstraint : public Constraint
{
public:
	#define DEFAULT_MOUSE_KS 10
	#define DEFAULT_MOUSE_KD 1

	MouseSpringConstraint();
	MouseSpringConstraint(int inParticle, hduVector3Dd inPosition);
	MouseSpringConstraint(int inParticle);
	virtual ~MouseSpringConstraint();

	virtual void ApplyConstraint(ParticleListT &particles);
	virtual void Draw(ParticleListT &particles);

	virtual void FlexToSystem(ParticleListT &particles) { }

	void SetState(bool inState) { mState = inState; }
	void SetPosition(hduVector3Dd inPosition) { mPosition = inPosition; }
	void SetParticle(int inParticle) { mParticle = inParticle; }
	
	int GetParticle(void) { return mParticle; }

private:
	int mParticle;
	hduVector3Dd mPosition;
	float mLength;	// usually 0
	float ks;		// spring stiffness
	float kd;		// damping constant
	
	bool mState; // on or off
};

#endif // !defined(AFX_MOUSESPRINGCONSTRAINT_H__A6A078ED_7494_45BF_8CF7_CD62301E3429__INCLUDED_)
