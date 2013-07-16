/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynPlane.h

Description:

  Base class for planes in dynamics scene.
*******************************************************************************/


#if !defined(AFX_DYNPLANE_H__75078DE9_FF7E_4161_A625_231EA2F75BFA__INCLUDED_)
#define AFX_DYNPLANE_H__75078DE9_FF7E_4161_A625_231EA2F75BFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicsMath.h"

class DynPlane  
{
public:

	virtual hduVector3Dd *getNormal(void) = 0;
	virtual hduVector3Dd *getFirstVertex(void) = 0;
};

#endif // !defined(AFX_DYNPLANE_H__75078DE9_FF7E_4161_A625_231EA2F75BFA__INCLUDED_)
