/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hdu.cpp

Description: 

  Helper routines for working with the Haptic Device Interface.

*******************************************************************************/

#include "hduAfx.h"

#include <cassert>

#include <HDU/hduMatrix.h>
#include <HDU/hduMath.h>
#include <HD/hd.h>
#include <HDU/hdu.h>

#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/glu.h>
#elif defined(__APPLE__)
# include <OpenGL/glu.h>
#endif

/******************************************************************************
 A utility routine for computing the uniform scale of the workspace
******************************************************************************/
HDdouble computeWorkspaceToViewVolumeScale(const HDdouble *projMatrix,
                                           const HDdouble *wsBounds,
                                           HDdouble zNear, HDdouble zFar)
{
    hduMatrix eyeTclip(projMatrix);
    hduMatrix clipTeye = eyeTclip;
    bool bNoError = clipTeye.invert();
    assert(bNoError);

    /* Make sure the zNear and zFar values are valid and then convert to clip 
       coordinates. */
    zNear = hduClamp(zNear, 0.0, 1.0);
    HDdouble zClipNear = hduLerp<HDdouble>(-1, 1, zNear);

    zFar = hduClamp(zFar, 0.0, 1.0);
    HDdouble zClipFar = hduLerp<HDdouble>(-1, 1, zFar);

    /* Compute the edges of the view frustum by transforming canonical
       coordinates for the corners into eye space. */
    hduVector3Dd eyeLeftBottomNear, eyeLeftBottomFar;
    hduVector3Dd eyeRightBottomNear, eyeRightBottomFar;
    hduVector3Dd eyeRightTopNear, eyeRightTopFar;
    hduVector3Dd eyeLeftTopNear, eyeLeftTopFar;

    hduVector3Dd clipLeftBottomNear(-1, -1, zClipNear);
    hduVector3Dd clipLeftBottomFar(-1, -1, zClipFar);
    hduVector3Dd clipRightBottomNear(1, -1, zClipNear);
    hduVector3Dd clipRightBottomFar(1, -1, zClipFar);
    hduVector3Dd clipRightTopNear(1, 1, zClipNear);
    hduVector3Dd clipRightTopFar(1, 1, zClipFar);
    hduVector3Dd clipLeftTopNear(-1, 1, zClipNear);
    hduVector3Dd clipLeftTopFar(-1, 1, zClipFar);

    clipTeye.multVecMatrix(clipLeftBottomNear, eyeLeftBottomNear);
    clipTeye.multVecMatrix(clipLeftBottomFar, eyeLeftBottomFar);
    clipTeye.multVecMatrix(clipRightBottomNear, eyeRightBottomNear);
    clipTeye.multVecMatrix(clipRightBottomFar, eyeRightBottomFar);
    clipTeye.multVecMatrix(clipRightTopNear, eyeRightTopNear);
    clipTeye.multVecMatrix(clipRightTopFar, eyeRightTopFar);
    clipTeye.multVecMatrix(clipLeftTopNear, eyeLeftTopNear);
    clipTeye.multVecMatrix(clipLeftTopFar, eyeLeftTopFar);    

    /* Evaluate the frustum dimensions at zFar. */
    hduVector3Dd leftBottom = eyeLeftBottomFar;
    hduVector3Dd rightBottom = eyeRightBottomFar;
    hduVector3Dd rightTop = eyeRightTopFar;
    hduVector3Dd leftTop = eyeLeftTopFar;

    HDdouble vvWidth = rightBottom[0] - leftBottom[0];
    HDdouble vvHeight = leftTop[1] - leftBottom[1];
    HDdouble vvDepth = eyeLeftBottomNear[2] - eyeLeftBottomFar[2];

    HDdouble wsWidth = wsBounds[3] - wsBounds[0];
    HDdouble wsHeight = wsBounds[4] - wsBounds[1];
    HDdouble wsDepth = wsBounds[5] - wsBounds[2];

    /* Determine the uniform scale as the minimum scale from
       workspace to view volume */
    HDdouble widthScale = vvHeight / wsHeight;
    HDdouble heightScale = vvWidth / wsWidth;
    HDdouble depthScale = vvDepth / wsDepth;

    HDdouble scale = hduMin(widthScale, heightScale);

    if (depthScale > DBL_EPSILON)
    {
        scale = (scale + depthScale) / 2.0;
    }

    return scale;
}

/******************************************************************************
 hduMapWorkspaceModelEx

 Determines a mapping for the haptic device's workspace so that it optimally
 fits within the viewing volume provided and is aligned with the view
 
 Parameters:
 viewMatrix   A pointer to a 4x4 modelview matrix stored in column-major order.
 projMatrix   A pointer to a 4x4 projection matrix stored in column-major order
 zNear        Value in the range 0 to 1 for mapping the front most z-extent of
              the haptic device.
 zFar         Value in the range 0 to 1 for specifying the depth at which the 
              scale of the workspace should be determined.
 wsModelMatrix A pointer to a 4x4 matrix used for storing the resultant
               column-major order workspacemodel transform.

 The workspaceview transform will transform coordinates from the haptic device
 coordinate space into world coordinates as seen by the camera.

 The viewMatrix and projMatrix arguments for this function are identical in
 form and purpose to the viewing transforms used by OpenGL.
******************************************************************************/
void hduMapWorkspaceModelEx(const HDdouble *modelMatrix,
                            const HDdouble *projMatrix,
                            HDdouble zNear, HDdouble zFar,
                            HDdouble *wsModelMatrix)
{
    hduMatrix worldTeye(modelMatrix);
    hduMatrix eyeTworld = worldTeye;
    bool bNoError = eyeTworld.invert();
    assert(bNoError);

    HDdouble aUsableWorkspace[6];
    HDdouble aMaxWorkspace[6];
    hdGetDoublev(HD_USABLE_WORKSPACE_DIMENSIONS, aUsableWorkspace);
    hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, aMaxWorkspace);

    HDdouble aWorkspaceBounds[6];
    for (int i = 0; i < 6; i++)
    {
        aWorkspaceBounds[i] = (aUsableWorkspace[i] + aMaxWorkspace[i]) / 2.0;
    }

    hduMatrix workspaceTeye;
    hduMapWorkspace(projMatrix, aWorkspaceBounds, zNear, zFar, workspaceTeye);

    /* Now apply the eyeTworld transform to the workspace transform
       (i.e. workspaceTeye * eyeTworld). */
    hduMatrix workspaceTworld = workspaceTeye;
    workspaceTworld.multRight(eyeTworld);

    memcpy(wsModelMatrix, workspaceTworld, 16 * sizeof(HDdouble));
}


/******************************************************************************
 hduMapWorkspaceModel

 Determines a mapping for the haptic device's workspace so that it optimally
 fits within the viewing volume provided and is aligned with the view.
  
 Parameters:
 modelMatrix A pointer to a 4x4 modelview matrix stored in column-major order.
 projMatrix  A pointer to a 4x4 projection matrix stored in column-major order.
 wsViewMatrix A pointer to a 4x4 matrix used for storing the resultant
              column-major order workspaceview transform.
******************************************************************************/
void hduMapWorkspaceModel(const HDdouble *modelMatrix,
                          const HDdouble *projMatrix,
                          HDdouble *wsModelMatrix)
{
    hduMatrix worldTeye(modelMatrix);
    hduMatrix eyeTworld = worldTeye;
    bool bNoError = eyeTworld.invert();
    assert(bNoError);

    HDdouble aUsableWorkspace[6];
    HDdouble aMaxWorkspace[6];
    hdGetDoublev(HD_USABLE_WORKSPACE_DIMENSIONS, aUsableWorkspace);
    hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, aMaxWorkspace);

    HDdouble aWorkspaceBounds[6];
    for (int i = 0; i < 6; i++)
    {
        aWorkspaceBounds[i] = (aUsableWorkspace[i] + aMaxWorkspace[i]) / 2.0;
    }

    /* Scale based on the entire extent of the view volume */
    HDdouble zNear = 0.0;
    HDdouble zFar = 1.0;

    hduMatrix workspaceTeye;
    hduMapWorkspace(projMatrix, aWorkspaceBounds, zNear, zFar, workspaceTeye);

    /* Now apply the eyeTworld transform to the workspace transform
       (i.e. workspaceTeye * eyeTworld). */
    hduMatrix workspaceTworld = workspaceTeye;
    workspaceTworld.multRight(eyeTworld);

    memcpy(wsModelMatrix, workspaceTworld, 16 * sizeof(HDdouble));
    
}
/*******************************************************************************
 hduMapWorkspace

 Determines a mapping for the haptic device's workspace so that it optimally
 fits within the viewing volume provided.

 Parameters:
 projMatrix   A pointer to a 4x4 projection matrix stored in column-major order.
 wsBounds     A pointer to a 6 element vector describing the bounding box of the
              workspace to be fit. xMin, yMin, zMin, xMax, yMax, zMax
 wsMatrix     A pointer to a 4x4 matrix used for storing the resultant 
              column-major order workspace transform.
 The workspace transform will transform coordinates from the haptic device
 coordinates space into eye coordinates of the viewing volume
*******************************************************************************/
void hduMapWorkspace(const HDdouble *projMatrix,
                     const HDdouble *wsBounds,
                     HDdouble zNear, HDdouble zFar,
                     HDdouble *wsMatrix)
{
    hduMatrix eyeTclip(projMatrix);
    hduMatrix clipTeye = eyeTclip;
    bool bNoError = clipTeye.invert();
    assert(bNoError);

    /* Make sure the zNear and zFar values are valid and then convert to clip 
       coordinates. */
    zNear = hduClamp(zNear, 0.0, 1.0);
    HDdouble zClipNear = hduLerp<HDdouble>(-1, 1, zNear);

    zFar = hduClamp(zFar, 0.0, 1.0);
    HDdouble zClipFar = hduLerp<HDdouble>(-1, 1, zFar);

    HDdouble scale = computeWorkspaceToViewVolumeScale(
        projMatrix, wsBounds, zNear, zFar);

    hduVector3Dd wsCenter;
    wsCenter[0] = (wsBounds[0] + wsBounds[3]) / 2.0;
    wsCenter[1] = (wsBounds[1] + wsBounds[4]) / 2.0;
    wsCenter[2] = (wsBounds[2] + wsBounds[5]) / 2.0;

    hduMatrix scaleWorkspace = hduMatrix::createScale(scale, scale, scale);
    hduMatrix toWorkspaceCenter = hduMatrix::createTranslation(-1.0 * wsCenter);
    hduMatrix fromWorkspaceCenter = hduMatrix::createTranslation(wsCenter);
    scaleWorkspace.multLeft(toWorkspaceCenter);
    scaleWorkspace.multRight(fromWorkspaceCenter);

    /* Matchup the center of the front of the workspace with the center of the 
       near plane. */
    hduVector3Dd wsCenterFront(wsCenter[0], wsCenter[1], wsBounds[5]);
    hduVector3Dd wsCenterFrontScaled;
    scaleWorkspace.multVecMatrix(wsCenterFront, wsCenterFrontScaled);
        
    hduVector3Dd vvCenterNearPlane;
    clipTeye.multVecMatrix(hduVector3Dd(0, 0, zClipNear), vvCenterNearPlane);

    /* Translate workspace so that the center of its front plane matches the 
       near plane of the view volume. */
    hduMatrix translateWorkspace = hduMatrix::createTranslation(
        vvCenterNearPlane - wsCenterFrontScaled);

    /* Finally, multiply the scale and translation to arrive at a transform
       from workspace coordinates to eye coordinates. */
    hduMatrix workspaceTeye = scaleWorkspace;
    workspaceTeye.multRight(translateWorkspace);

    /* Copy the result into the destination workspace transform matrix. */
    memcpy(wsMatrix, workspaceTeye, 16 * sizeof(HDdouble));
}

/******************************************************************************
 hduScreenToWorkspaceScale

 Computes the uniform scale factor that will map screen coordinate scale
 into workspace coordinates. This can be used for determining a visual scale
 for an entity that lives in workspace coordinates, like the 3D cursor.
 *****************************************************************************/
HDdouble hduScreenToWorkspaceScale(const HDdouble *modelMatrix,
                                   const HDdouble *projMatrix,
                                   const HDint *viewport,
                                   const HDdouble *wsViewMatrix)
{
    hduVector3Dd p0, p1;

    hduMatrix worldTeye(modelMatrix);
    hduMatrix eyeTclip(projMatrix);
    hduMatrix worldTclip = worldTeye;
    worldTclip.multRight(eyeTclip);

    hduMatrix clipTworld = worldTclip;
    bool bNoError = clipTworld.invert();
    assert(bNoError);

    static const hduVector3Dd leftBottomNearClip(-1, -1, -1);
    static const hduVector3Dd rightTopNearClip(1, 1, -1);

    /* First compute the uniform scale factor between screen coordinates and
       world coordinates. */
    clipTworld.multVecMatrix(leftBottomNearClip, p0);
    clipTworld.multVecMatrix(rightTopNearClip, p1);  

    HDdouble screenDiagonal = sqrt(double(viewport[2] * viewport[2] + 
                                   viewport[3] * viewport[3]));

    HDdouble screenTworldScale = (p0 - p1).magnitude() / screenDiagonal;

    /* Now determine the scale factor from world to workspace coordinates. */
    hduMatrix workspaceTworld(wsViewMatrix);

    workspaceTworld.multVecMatrix(hduVector3Dd(0, 0, 0), p0);
    workspaceTworld.multVecMatrix(hduVector3Dd(1, 1, 1), p1);

    HDdouble workspaceTworldScale = (p0 - p1).magnitude() / sqrt(3.0);

    HDdouble screenTworkspaceScale = screenTworldScale / workspaceTworldScale;

    return screenTworkspaceScale;
}
