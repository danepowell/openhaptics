/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  DynFreePlane.h

Description:

  Free plane in dynamics scene (not part of a body).

*******************************************************************************/

#if !defined(AFX_DYNFREEPLANE_H__022D3A57_F464_4E1D_A1FC_CC666DE38BD5__INCLUDED_)
#define AFX_DYNFREEPLANE_H__022D3A57_F464_4E1D_A1FC_CC666DE38BD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynPlane.h"
class DynEdge;

class DynFreePlane : public DynPlane  
{
public:
    DynFreePlane();
    virtual ~DynFreePlane();

    virtual hduVector3Dd *getNormal(void);
    virtual hduVector3Dd *getFirstVertex(void);

    void setEdges(DynEdge *ep, DynEdge *es);
    void reverse(void);
    void recalculateNormal(void);

private:
    // return true if a valid plane can be obtained
    bool calculateNormal(hduVector3Dd &v0, hduVector3Dd &v1, hduVector3Dd &v2, hduVector3Dd &v3);

    DynEdge* edgePri;
    DynEdge* edgeSec;
    hduVector3Dd mNormal;
    bool mValid;
    bool mReverse;
};

#endif // !defined(AFX_DYNFREEPLANE_H__022D3A57_F464_4E1D_A1FC_CC666DE38BD5__INCLUDED_)

/******************************************************************************/
