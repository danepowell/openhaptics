/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hlu.h

Description: 

  Utility functions for HLAPI.

*****************************************************************************/

#ifndef hlu_H_
#define hlu_H_

#include <HL/hlTypes.h>
#include <HL/hlDefines.h>

#ifdef __cplusplus
extern "C" 
{
#endif

void hluFitWorkspace(const HLdouble projMatrix[16]);

void hluFitWorkspaceFront(const HLdouble projMatrix[16]);

void hluFitWorkspaceNonUniform(const HLdouble projMatrix[16]);

void hluFitWorkspaceBox(
    const HLdouble modelMatrix[16],
    const HLdouble minPt[3],
    const HLdouble maxPt[3]);

void hluFitWorkspaceBoxFront(
    const HLdouble modelMatrix[16],
    const HLdouble minPt[3],
    const HLdouble maxPt[3]);

void hluFitWorkspaceBoxNonUniform(
    const HLdouble modelMatrix[16],
    const HLdouble minPt[3],
    const HLdouble maxPt[3]);

void hluFeelFrom (
    HLdouble handx, 
    HLdouble handy, 
    HLdouble handz, 
    HLdouble centerx, 
    HLdouble centery, 
    HLdouble centerz, 
    HLdouble upx, 
    HLdouble upy, 
    HLdouble upz);

HLdouble hluScreenToModelScale(
    const HLdouble modelMatrix[16],
    const HLdouble projMatrix[16],
    const HLint viewport[4]);

void hluModelToWorkspaceScale(
    const HLdouble modelMatrix[16],
    const HLdouble viewTouchMatrix[16],
    const HLdouble touchWorkspaceMatrix[16],
    HLdouble scale[3]);

void hluModelToWindowTransform(
    const HLdouble modelMatrix[16],
    const HLdouble projMatrix[16],
    const HLint viewport[4],
    HLdouble modelWindowMatrix[16]);

void hluModelToWorkspaceTransform(
    const HLdouble modelMatrix[16],
    const HLdouble viewTouchMatrix[16],
    const HLdouble touchWorkspaceMatrix[16],
    HLdouble modelWorkspaceMatrix[16]);

void hluScaleGimbalAngles(HLdouble scaleX, HLdouble scaleY, 
                          HLdouble scaleZ, HLdouble nT[16]);

#ifdef __cplusplus
}
#endif

#endif  /* hlu_H_ */

/******************************************************************************/
