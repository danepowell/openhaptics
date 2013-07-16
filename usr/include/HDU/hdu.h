/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hdu.h

Description: 

  Helper routines for working with the Haptic Device Interface.
  Facilitates alignement of haptics and graphics coordinate frames.

*******************************************************************************/
#ifndef hdu_H_
#define hdu_H_

#include <HD/hd.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 hduMapWorkspaceModel

 Determines a mapping for the haptic device's workspace so that it optimally
 fits within the viewing volume provided and is aligned with the view.
  
 Parameters:
 modelMatrix A pointer to a 4x4 modelview matrix stored in column-major order.
 projMatrix  A pointer to a 4x4 projection matrix stored in column-major order.
 wsModelMatrix A pointer to a 4x4 matrix used for storing the resultant
               column-major order workspacemodel transform.
******************************************************************************/
void hduMapWorkspaceModel(const HDdouble *modelMatrix,
                          const HDdouble *projMatrix,
                          HDdouble *wsModelMatrix);

/******************************************************************************
 hduMapWorkspaceModel

 Determines a mapping for the haptic device's workspace so that it optimally
 fits within the viewing volume provided and is aligned with the view.
 
 Parameters:
 modelMatrix  A pointer to a 4x4 modelview matrix stored in column-major order.
 projMatrix   A pointer to a 4x4 projection matrix stored in column-major order
 zNear        Value in the range 0 to 1 for mapping the front most z-extent of
              the haptic device.
 zFar         Value in the range 0 to 1 for specifying the depth at which the 
              scale of the workspace should be determined.
 wsModelMatrix A pointer to a 4x4 matrix used for storing the resultant
               column-major order workspacemodel transform.

 The workspacemodel transform will transform coordinates from the haptic device
 coordinate space into world coordinates as seen by the camera.

 The modelMatrix and projMatrix arguments for this function are identical in
 form and purpose to the viewing transforms used by OpenGL.
******************************************************************************/
void hduMapWorkspaceModelEx(const HDdouble *modelMatrix,
                            const HDdouble *projMatrix,
                            HDdouble zNear, HDdouble zFar,
                            HDdouble *wsModelMatrix);

/******************************************************************************
 hduMapWorkspace

 Determines a mapping for the haptic device's workspace so that it optimally
 fits within the viewing volume provided.
 
 Parameters:
 projMatrix   A pointer to a 4x4 projection matrix stored in column-major order
 wsBounds     A pointer to a 6 element vector describing the bounding box of the
              workspace to be fit. xMin, yMin, zMin, xMax, yMax, zMax
 zNear        Value in the range 0 to 1 for mapping the front most z-extent of
              the haptic device.
 zFar         Value in the range 0 to 1 for specifying the depth at which the 
              scale of the workspace should be determined.
 wsMatrix     A pointer to a 4x4 matrix used for storing the resultant column-
              major order workspace transform.

 The workspace transform will transform coordinates from the haptic device
 coordinates space into eye coordinates of the viewing volume.
******************************************************************************/
void hduMapWorkspace(const HDdouble *projMatrix,
                     const HDdouble *wsBounds,
                     HDdouble zNear, HDdouble zFar,
                     HDdouble *wsMatrix);

/******************************************************************************
 hduScreenToWorkspaceScale

 Computes the uniform scale factor that will map screen coordinate scale
 into workspace coordinates. This can be used for determining a visual scale
 for an entity that lives in workspace coordinates, like the 3D cursor.

 The arguments for this function are identical in form and purpose to the
 viewing transforms used by OpenGL.
******************************************************************************/
HDdouble hduScreenToWorkspaceScale(const HDdouble *modelMatrix,
                                   const HDdouble *projMatrix,
                                   const HDint *viewport,
                                   const HDdouble *wsViewMatrix);

#ifdef __cplusplus
}
#endif

#endif /* hdu_H_ */

/*****************************************************************************/
