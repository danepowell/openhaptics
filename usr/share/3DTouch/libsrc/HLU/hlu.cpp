/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hlu.cpp

Description: 
  
  Utility functions for HLAPI

*******************************************************************************/

#include "hluAfx.h"

#include <cassert>

#include <HDU/hduMatrix.h>
#include <HDU/hduBoundBox.h>
#include <HL/hl.h>
#include <HLU/hlu.h>
#include <HDU/hdu.h>
#include <HDU/hduMath.h>

// transform clip coordinates to touch coordinates
void frustumClipToTouchBounds(
    const HDdouble projMatrix[16],
    hduVector3Dd& touchLeftBottomNear,
    hduVector3Dd& touchLeftBottomFar,
    hduVector3Dd& touchRightBottomNear, 
    hduVector3Dd& touchRightBottomFar,
    hduVector3Dd& touchRightTopNear, 
    hduVector3Dd& touchRightTopFar,
    hduVector3Dd& touchLeftTopNear, 
    hduVector3Dd& touchLeftTopFar)
{
    HDdouble zClipNear = -1;
    HDdouble zClipFar = 1;

    hduVector3Dd clipLeftBottomNear(-1, -1, zClipNear);
    hduVector3Dd clipLeftBottomFar(-1, -1, zClipFar);
    hduVector3Dd clipRightBottomNear(1, -1, zClipNear);
    hduVector3Dd clipRightBottomFar(1, -1, zClipFar);
    hduVector3Dd clipRightTopNear(1, 1, zClipNear);
    hduVector3Dd clipRightTopFar(1, 1, zClipFar);
    hduVector3Dd clipLeftTopNear(-1, 1, zClipNear);
    hduVector3Dd clipLeftTopFar(-1, 1, zClipFar);

    hduMatrix eyeTclip(projMatrix);
    hduMatrix clipTeye = eyeTclip;
    bool bNoError = clipTeye.invert();
    assert(bNoError);

    HDdouble viewTouchMatrix[16];
    hlGetDoublev(HL_VIEWTOUCH_MATRIX, viewTouchMatrix);

    hduMatrix clipTtouch = clipTeye * hduMatrix(viewTouchMatrix);

    clipTtouch.multVecMatrix(clipLeftBottomNear, touchLeftBottomNear);
    clipTtouch.multVecMatrix(clipLeftBottomFar, touchLeftBottomFar);
    clipTtouch.multVecMatrix(clipRightBottomNear, touchRightBottomNear);
    clipTtouch.multVecMatrix(clipRightBottomFar, touchRightBottomFar);
    clipTtouch.multVecMatrix(clipRightTopNear, touchRightTopNear);
    clipTtouch.multVecMatrix(clipRightTopFar, touchRightTopFar);
    clipTtouch.multVecMatrix(clipLeftTopNear, touchLeftTopNear);
    clipTtouch.multVecMatrix(clipLeftTopFar, touchLeftTopFar);
}


//////////////////////////////////////////////////////////////////////////////
//  hluFitWorkspace -
//     Generates mapping for the haptic device's workspace so that it 
//     optimally (and uniformly) fits within the center of the viewing volume.
//
//     The matrix generated (hlOrtho) is multiplied by the current matrix.  
//
// Parameters: projMatrix A 4x4 projection matrix stored in column-major order.
//////////////////////////////////////////////////////////////////////////////
void hluFitWorkspace(const HLdouble projMatrix[16])
{
    // extract bounds of the view volume
    hduVector3Dd touchLeftBottomNear, touchLeftBottomFar;
    hduVector3Dd touchRightBottomNear, touchRightBottomFar;
    hduVector3Dd touchRightTopNear, touchRightTopFar;
    hduVector3Dd touchLeftTopNear, touchLeftTopFar;

    frustumClipToTouchBounds(projMatrix,
                             touchLeftBottomNear, touchLeftBottomFar,
                             touchRightBottomNear, touchRightBottomFar,
                             touchRightTopNear, touchRightTopFar,
                             touchLeftTopNear, touchLeftTopFar);

    hduBoundBox3Dd box;
    box.Union(touchLeftBottomNear);
    box.Union(touchRightBottomNear);
    box.Union(touchRightTopNear);
    box.Union(touchLeftTopNear);
    box.Union(touchLeftBottomFar);
    box.Union(touchRightBottomFar);
    box.Union(touchRightTopFar);
    box.Union(touchLeftTopFar);

    hduVector3Dd boxExtents = box.hi() - box.lo();
    
    HDdouble wsBoundsHL[6];
    hlGetDoublev(HL_WORKSPACE, wsBoundsHL);

    hduVector3Dd wsExtents(wsBoundsHL[3] - wsBoundsHL[0],
                           wsBoundsHL[4] - wsBoundsHL[1],
                           wsBoundsHL[5] - wsBoundsHL[2]);

    // uniform of workspace to match box in touch coordinates
    hduVector3Dd scale = boxExtents / wsExtents;
    HDdouble uniformScale = (hduMax(scale[0],scale[1])+scale[2])/2.0;
    
    scale[0] = uniformScale;
    scale[1] = uniformScale;
    scale[2] = uniformScale;

    hduVector3Dd wsCenter((wsBoundsHL[0] + wsBoundsHL[3])/2.0,
                          (wsBoundsHL[1] + wsBoundsHL[4])/2.0,
                          (wsBoundsHL[2] + wsBoundsHL[5])/2.0);
    
    hduVector3Dd boxCenter((box.lo()+box.hi())/2.0);

    hduMatrix toWSCenter = hduMatrix::createTranslation(-wsCenter);
    hduMatrix wsScale = hduMatrix::createScale(hduVector3Dd(scale));
    hduMatrix toBoxCenter = hduMatrix::createTranslation(boxCenter);
    
    wsScale.multLeft(toWSCenter);
    wsScale.multRight(toBoxCenter);

    hlMultMatrixd(wsScale.getInverse());
}

//////////////////////////////////////////////////////////////////////////////
//  hluFitWorkspaceFront -
//     Generates mapping for the haptic device's workspace so that it 
//     optimally (and uniformly) fits within the viewing volume provided 
//     and is aligned with the view at the near plane.  The matrix generated 
//     (hlOrtho) is multiplied by the current matrix.  
//
// This mapping routine is most useful for cases where you need to ensure that
// the front plane is touchable. For instance, allowing the user to always feel
// what is closest in the view, or computing the mapping so that the user can
// operate the haptic mouse outside of the viewport.
//
// Parameters: projMatrix A 4x4 projection matrix stored in column-major order.
//////////////////////////////////////////////////////////////////////////////
void hluFitWorkspaceFront(const HLdouble projMatrix[16])
{
    // extract bounds of the view volume
    hduVector3Dd touchLeftBottomNear, touchLeftBottomFar;
    hduVector3Dd touchRightBottomNear, touchRightBottomFar;
    hduVector3Dd touchRightTopNear, touchRightTopFar;
    hduVector3Dd touchLeftTopNear, touchLeftTopFar;

    frustumClipToTouchBounds(projMatrix,
                             touchLeftBottomNear, touchLeftBottomFar,
                             touchRightBottomNear, touchRightBottomFar,
                             touchRightTopNear, touchRightTopFar,
                             touchLeftTopNear, touchLeftTopFar);

    hduBoundBox3Dd box;
    box.Union(touchLeftBottomNear);
    box.Union(touchRightBottomNear);
    box.Union(touchRightTopNear);
    box.Union(touchLeftTopNear);
    box.Union(touchLeftBottomFar);
    box.Union(touchRightBottomFar);
    box.Union(touchRightTopFar);
    box.Union(touchLeftTopFar);

    hduVector3Dd boxExtents = box.hi() - box.lo();
    
    HDdouble wsBoundsHL[6];
    hlGetDoublev(HL_WORKSPACE, wsBoundsHL);

    hduVector3Dd wsExtents(wsBoundsHL[3] - wsBoundsHL[0],
                           wsBoundsHL[4] - wsBoundsHL[1],
                           wsBoundsHL[5] - wsBoundsHL[2]);

    // Fit the workspace extents to a canonical volume by fitting the min
    // dimension (typically Z)
    HDdouble wsToCan = 1.0 / hduMin(hduMin(wsExtents[0], wsExtents[1]), wsExtents[2]);

    // Fit the box extents to a canonical volume by fitting the max dimension
    HDdouble canToBox = hduMax(hduMax(boxExtents[0], boxExtents[1]), boxExtents[2]);

    HDdouble scaleWsToBox = wsToCan * canToBox;
    
    hduVector3Dd wsFrontCenter((wsBoundsHL[0] + wsBoundsHL[3])/2.0,
                               (wsBoundsHL[1] + wsBoundsHL[4])/2.0,
                               wsBoundsHL[5]);
    
    hduVector3Dd boxFrontCenter((box.lo()+box.hi())/2.0);
    boxFrontCenter[2] = box.hi()[2];

    hduMatrix toWSFrontCenter = hduMatrix::createTranslation(-wsFrontCenter);
    hduMatrix wsScale = hduMatrix::createScale(scaleWsToBox, scaleWsToBox, scaleWsToBox);
    hduMatrix toBoxFrontCenter = hduMatrix::createTranslation(boxFrontCenter);
    
    wsScale.multLeft(toWSFrontCenter);
    wsScale.multRight(toBoxFrontCenter);

    hlMultMatrixd(wsScale.getInverse());
}

//////////////////////////////////////////////////////////////////////////////
//  hluFitWorkspaceNonUniform -
//     Generates mapping for the haptic device's workspace so that it 
//     optimally fits the viewing volume provided and is aligned with 
//     the view.  The matrix generated (hlOrtho) is multiplied by the 
//     current matrix.  
// 
// Parameters: projMatrix A 4x4 projection matrix stored in column-major order.
//////////////////////////////////////////////////////////////////////////////
void hluFitWorkspaceNonUniform(const HLdouble projMatrix[16])
{
    // extract bounds of the view volume
    hduVector3Dd touchLeftBottomNear, touchLeftBottomFar;
    hduVector3Dd touchRightBottomNear, touchRightBottomFar;
    hduVector3Dd touchRightTopNear, touchRightTopFar;
    hduVector3Dd touchLeftTopNear, touchLeftTopFar;

    frustumClipToTouchBounds(projMatrix,
                             touchLeftBottomNear, touchLeftBottomFar,
                             touchRightBottomNear, touchRightBottomFar,
                             touchRightTopNear, touchRightTopFar,
                             touchLeftTopNear, touchLeftTopFar);

    hduBoundBox3Dd box;
    box.Union(touchLeftBottomNear);
    box.Union(touchLeftBottomFar);
    box.Union(touchRightBottomNear);
    box.Union(touchRightBottomFar);
    box.Union(touchRightTopNear);
    box.Union(touchRightTopFar);
    box.Union(touchLeftTopNear);
    box.Union(touchLeftTopFar);

    hlOrtho(box.lo()[0], box.hi()[0], 
            box.lo()[1], box.hi()[1], 
            -box.hi()[2], -box.lo()[2]); // distance to near/far
}


//////////////////////////////////////////////////////////////////////////////
//  hluCommonFitWorkspaceBox - 
//     Auxilary routine to hluFitWorkspaceBox and hluFitWorkspaceBoxNonUniform.
//     Generates an orthographic projection matrix that enclosing the 
//     bounding box described by lowerLeftNear and upperRightFar transformed
//     by modelMatrix into view coordinates.
//  Parameters -
//        modelMatrix  - 4x4 matrix stored in column-major order.
//        minPt        - position
//        maxPt        - position
//////////////////////////////////////////////////////////////////////////////
void hluCommonFitWorkspaceBox(
    HLdouble projMatrix[16],
    const HLdouble modelMatrix[16],
    const HLdouble minPt[3],
    const HLdouble maxPt[3])
{
    hduBoundBox3Dd box;
    hduMatrix modelTeye(modelMatrix);

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
            {
                hduVector3Dd pos;
                pos[0] = i ? minPt[0] : maxPt[0];
                pos[1] = j ? minPt[1] : maxPt[1];
                pos[2] = k ? minPt[2] : maxPt[2];

                hduVector3Dd posView;
                modelTeye.multVecMatrix(pos, posView);

                box.Union(posView);
            }

    HDdouble left = box.lo()[0];
    HDdouble right = box.hi()[0];

    HDdouble bottom = box.lo()[1];
    HDdouble top = box.hi()[1];

    HDdouble nearDist = -box.hi()[2];
    HDdouble farDist = -box.lo()[2];

    projMatrix[0] = 2.0 / (right-left);
    projMatrix[4] = 0.0;
    projMatrix[8] = 0.0;
    projMatrix[12] = -(right+left)/(right-left);
    
    projMatrix[1] = 0.0;
    projMatrix[5] = 2.0 / (top-bottom);
    projMatrix[9] = 0.0;
    projMatrix[13] = -(top+bottom)/(top-bottom);

    projMatrix[2] = 0.0;
    projMatrix[6] = 0.0;
    projMatrix[10] = -2.0 / (farDist-nearDist);
    projMatrix[14] = -(farDist+nearDist)/(farDist-nearDist);

    projMatrix[3] = 0.0;
    projMatrix[7] = 0.0;
    projMatrix[11] = 0.0;
    projMatrix[15] = 1.0;
}


//////////////////////////////////////////////////////////////////////////////
//  hluFitWorkspaceBox -
//     Generates a uniform mapping for the haptic device's workspace 
//     so that it optimally fits within the center of the box defined 
//     by lowerLeftNear and upperRightFar.
//
//     modelMatrix transforms box to view coordinates.
//
//  Parameters -
//        modelMatrix   - 4x4 matrix stored in column-major order.
//        lowerLeftNear - position
//        upperRightFar - position
//////////////////////////////////////////////////////////////////////////////
void hluFitWorkspaceBox(
    const HLdouble modelMatrix[16],
    const HLdouble minPt[3],
    const HLdouble maxPt[3])
{
    HLdouble projMatrix[16];
    hluCommonFitWorkspaceBox(projMatrix, modelMatrix,
                             minPt, maxPt);

    hluFitWorkspace(projMatrix);
}

//////////////////////////////////////////////////////////////////////////////
//  hluFitWorkspaceBoxFront -
//     Generates a uniform mapping for the haptic device's workspace 
//     so that it optimally fits the front of the box defined by lowerLeftNear
//     and upperRightFar.
//
//  modelMatrix transforms box to view coordinates.
//
// This mapping routine is most useful for cases where you need to ensure that
// the front plane is touchable. For instance, allowing the user to always feel
// what is closest in the view, or computing the mapping so that the user can
// operate the haptic mouse outside of the viewport.
//
//  Parameters -
//        modelMatrix   - 4x4 matrix stored in column-major order.
//        lowerLeftNear - position
//        upperRightFar - position
//////////////////////////////////////////////////////////////////////////////
void hluFitWorkspaceBoxFront(
    const HLdouble modelMatrix[16],
    const HLdouble minPt[3],
    const HLdouble maxPt[3])
{
    HLdouble projMatrix[16];
    hluCommonFitWorkspaceBox(projMatrix, modelMatrix,
                             minPt, maxPt);

    hluFitWorkspaceFront(projMatrix);
}

//////////////////////////////////////////////////////////////////////////////
//  hluFitWorkspaceBoxNonUniform -
//     Generates a non-uniform mapping for the haptic device's workspace 
//     so that it optimally fits a box defined by lowerLeftNear and 
//     upperRightFar.  modelMatrix transforms box to view coordinates.
//  Parameters -
//        modelMatrix   - 4x4 matrix stored in column-major order.
//        lowerLeftNear - position
//        upperRightFar - position
//////////////////////////////////////////////////////////////////////////////
void hluFitWorkspaceBoxNonUniform(
    const HLdouble modelMatrix[16],
    const HLdouble minPt[3],
    const HLdouble maxPt[3])
{
    HLdouble projMatrix[16];
    hluCommonFitWorkspaceBox(projMatrix, modelMatrix,
                             minPt, maxPt);

    hluFitWorkspaceNonUniform(projMatrix);
}


//////////////////////////////////////////////////////////////////////////////
//  hluScreenToWorldScale -
//
//////////////////////////////////////////////////////////////////////////////
HLdouble hluScreenToModelScale(const HLdouble modelMatrix[16],
                               const HLdouble projMatrix[16],
                               const HLint viewport[4])
{
    hduVector3Dd p0, p1;

    hduMatrix modelTeye(modelMatrix);
    hduMatrix eyeTclip(projMatrix);
    hduMatrix modelTclip = modelTeye;
    modelTclip.multRight(eyeTclip);

    hduMatrix clipTmodel = modelTclip;
    bool bNoError = clipTmodel.invert();
    assert(bNoError);

    static const hduVector3Dd leftBottomNearClip(-1, -1, -1);
    static const hduVector3Dd rightTopNearClip(1, 1, -1);

    clipTmodel.multVecMatrix(leftBottomNearClip, p0);
    clipTmodel.multVecMatrix(rightTopNearClip, p1);  

    HDdouble screenDiagonal = sqrt(double(viewport[2] * viewport[2] + 
                                          viewport[3] * viewport[3]));

    HDdouble screenTmodelScale = (p0 - p1).magnitude() / screenDiagonal;

    return screenTmodelScale;
}


//////////////////////////////////////////////////////////////////////////////
//  hluModelToWorkspaceScale - 
//
//////////////////////////////////////////////////////////////////////////////
void hluModelToWorkspaceScale(
    const HLdouble modelMatrix[16],
    const HLdouble viewTouchMatrix[16],
    const HLdouble touchWorkspaceMatrix[16],
    HLdouble scale[3])
{
    hduMatrix modelTworkspace = 
        hduMatrix(modelMatrix) *
        hduMatrix(viewTouchMatrix) *
        hduMatrix(touchWorkspaceMatrix);

    hduMatrix workspaceTmodel = modelTworkspace.getInverse();

    hduVector3Dd origin, px, py, pz;
    workspaceTmodel.multVecMatrix(hduVector3Dd(0,0,0), origin);
    workspaceTmodel.multVecMatrix(hduVector3Dd(1,0,0), px);
    workspaceTmodel.multVecMatrix(hduVector3Dd(0,1,0), py);
    workspaceTmodel.multVecMatrix(hduVector3Dd(0,0,1), pz);

    // workspace to model scale (per coordinate)
    scale[0] = (origin-px).magnitude();
    scale[1] = (origin-py).magnitude();
    scale[2] = (origin-pz).magnitude();
}

//////////////////////////////////////////////////////////////////////////////
//  hluModelToWindowTransform -
//
//////////////////////////////////////////////////////////////////////////////
void hluModelToWindowTransform(
    const HLdouble modelMatrix[16],
    const HLdouble projMatrix[16],
    const HLint viewport[4],
    HLdouble modelWindowMatrix[16])
{
    // Compute the NDC to viewport transform
    hduMatrix T1 = hduMatrix::createTranslation(1.0, 1.0, 1.0);
    hduMatrix S1 = hduMatrix::createScale(
        viewport[2] / 2.0, viewport[3] / 2.0, 0.5);
    hduMatrix T2 = hduMatrix::createTranslation(viewport[0], viewport[1], 0);

    hduMatrix modelToWindow = hduMatrix(modelMatrix) *
                              hduMatrix(projMatrix) * 
                              T1 * S1 * T2;
    memcpy(modelWindowMatrix, modelToWindow, sizeof(HLdouble) * 16);    
}


//////////////////////////////////////////////////////////////////////////////
//  hluModelToWorkspaceTransform -
//
//////////////////////////////////////////////////////////////////////////////
void hluModelToWorkspaceTransform(
    const HLdouble modelMatrix[16],
    const HLdouble viewTouchMatrix[16],
    const HLdouble touchWorkspaceMatrix[16],
    HLdouble modelWorkspaceMatrix[16])
{
    hduMatrix modelToWorkspace = 
        hduMatrix(modelMatrix) *
        hduMatrix(viewTouchMatrix) *
        hduMatrix(touchWorkspaceMatrix);    

    memcpy(modelWorkspaceMatrix, modelToWorkspace, sizeof(HLdouble) * 16);
}


//////////////////////////////////////////////////////////////////////////////
//  hluFeelFrom -
//
//////////////////////////////////////////////////////////////////////////////
void hluFeelFrom (HLdouble handx, HLdouble handy, HLdouble handz, 
                  HLdouble centerx, HLdouble centery, HLdouble centerz,
                  HLdouble upx, HLdouble upy, HLdouble upz)
{
    // create vectors for rotation
    hduVector3Dd z(handx-centerx, handy-centery, handz-centerz);
    hduVector3Dd y(upx, upy, upz);
    hduVector3Dd x = y.crossProduct(z);

    x.normalize();
    y.normalize();
    z.normalize();

    hduMatrix mat(x[0], y[0], z[0], 0.0,
                  x[1], y[1], z[1], 0.0,
                  x[2], y[2], z[2], 0.0,
                  0.0,  0.0,  0.0,  1.0);

    hlMultMatrixd(mat);

    // move hand to origin
    hlTranslated(-handx, -handy, -handz);
}
//////////////////////////////////////////////////////////////////////////////
//  hluScaleGimbalAngles - Scale gimbal angles and send back the new transform. 
//
//////////////////////////////////////////////////////////////////////////////
void hluScaleGimbalAngles(HLdouble scaleX, HLdouble scaleY, 
                          HLdouble scaleZ, HLdouble nT[16])
{
    HLdouble newScaledTransform[16];
    // Use gimbal angle scaling transform from HD and replace 
    // the translation part with proxy position.
    hdScaleGimbalAngles(scaleX, scaleY, scaleZ, newScaledTransform);
    hduVector3Dd proxyPosition;
    hlGetDoublev(HL_PROXY_POSITION, proxyPosition);
    newScaledTransform[12] = proxyPosition[0];
    newScaledTransform[13] = proxyPosition[1];
    newScaledTransform[14] = proxyPosition[2];

    memcpy(nT,newScaledTransform,sizeof(newScaledTransform));
}