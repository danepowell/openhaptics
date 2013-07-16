/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

    hl.h

Description: 

    Procedure declarations, constant definitions and macros for the HLAPI.

*****************************************************************************/

#ifndef HL_H_DEFINE
#define HL_H_DEFINE

#include <HD/hd.h>

#include <HL/hlExport.h>
#include <HL/hlCompilerConfig.h>

#include <HL/hlTypes.h>
#include <HL/hlDefines.h>

#ifdef __cplusplus
extern "C" 
{
#endif

HLAPI HHLRC HLAPIENTRY hlCreateContext (HHD hHD);
HLAPI void HLAPIENTRY hlDeleteContext (HHLRC hHLRC);

HLAPI void HLAPIENTRY hlMakeCurrent (HHLRC hHLRC);
HLAPI void HLAPIENTRY hlContextDevice (HHD hHD);

HLAPI HHLRC HLAPIENTRY hlGetCurrentContext (void);
HLAPI HHD HLAPIENTRY hlGetCurrentDevice (void);

HLAPI void HLAPIENTRY hlBeginFrame (void);
HLAPI void HLAPIENTRY hlEndFrame (void);

/* State maintenance and accessors */
HLAPI void HLAPIENTRY hlEnable (HLenum cap);
HLAPI void HLAPIENTRY hlDisable (HLenum cap);
HLAPI HLboolean HLAPIENTRY hlIsEnabled (HLenum cap);
HLAPI void HLAPIENTRY hlGetBooleanv (HLenum pname, HLboolean *params);
HLAPI void HLAPIENTRY hlGetDoublev (HLenum pname, HLdouble *params);
HLAPI void HLAPIENTRY hlGetIntegerv (HLenum pname, HLint *params);
HLAPI HLerror HLAPIENTRY hlGetError (void);
HLAPI const HLubyte * HLAPIENTRY hlGetString (HLenum name);

/* Shape specific accessors */
HLAPI void HLAPIENTRY hlGetShapeBooleanv(HLuint shapeId, HLenum pname, HLboolean *params);
HLAPI void HLAPIENTRY hlGetShapeDoublev(HLuint shapeId, HLenum pname, HLdouble *params);

HLAPI void HLAPIENTRY hlHinti (HLenum target, HLint value);
HLAPI void HLAPIENTRY hlHintb (HLenum target, HLboolean value);

/* Cached state accessors */
HLAPI void HLAPIENTRY hlCacheGetBooleanv (HLcache *cache, HLenum pname, HLboolean *params);
HLAPI void HLAPIENTRY hlCacheGetDoublev (HLcache *cache, HLenum pname, HLdouble *params);

/* Shapes */
HLAPI HLuint HLAPIENTRY hlGenShapes (HLsizei range);
HLAPI void HLAPIENTRY hlDeleteShapes (HLuint shape, HLsizei range);
HLAPI HLboolean HLAPIENTRY hlIsShape (HLuint shape);
HLAPI void HLAPIENTRY hlBeginShape (HLenum type, HLuint shape);
HLAPI void HLAPIENTRY hlEndShape (void);
HLAPI void HLAPIENTRY hlCallShape (HLuint shape);

/* Materials and Surface properties */
HLAPI void HLAPIENTRY hlMaterialf (HLenum face, HLenum pname, HLfloat param);
HLAPI void HLAPIENTRY hlGetMaterialfv (HLenum face, HLenum pname, HLfloat *params);

HLAPI void HLAPIENTRY hlTouchableFace (HLenum mode);

HLAPI void HLAPIENTRY hlTouchModel (HLenum mode);
HLAPI void HLAPIENTRY hlTouchModelf (HLenum pname, HLfloat param);

/* Push and pop attributes */
HLAPI void HLAPIENTRY hlPushAttrib(HLbitfield mask);
HLAPI void HLAPIENTRY hlPopAttrib();
    
/* Force effects */
HLAPI HLuint HLAPIENTRY hlGenEffects (HLsizei range);
HLAPI void HLAPIENTRY hlDeleteEffects (HLuint effect, HLsizei range);
HLAPI HLboolean HLAPIENTRY hlIsEffect (HLuint effect);

HLAPI void HLAPIENTRY hlTriggerEffect (HLenum type);
HLAPI void HLAPIENTRY hlStartEffect (HLenum type, HLuint effect);
HLAPI void HLAPIENTRY hlStopEffect (HLuint effect);
HLAPI void HLAPIENTRY hlUpdateEffect(HLuint effect);

HLAPI void HLAPIENTRY hlEffectd (HLenum pname, HLdouble param);
HLAPI void HLAPIENTRY hlEffecti (HLenum pname, HLint param);
HLAPI void HLAPIENTRY hlEffectdv (HLenum pname, const HLdouble *params);
HLAPI void HLAPIENTRY hlEffectiv (HLenum pname, const HLint *params);

HLAPI void HLAPIENTRY hlGetEffectdv (HLuint effect, HLenum pname, HLdouble *params);
HLAPI void HLAPIENTRY hlGetEffectiv (HLuint effect, HLenum pname, HLint *params);
HLAPI void HLAPIENTRY hlGetEffectbv (HLuint effect, HLenum pname, HLboolean *param);
    
/* Proxy */
HLAPI void HLAPIENTRY hlProxydv (HLenum pname, const HLdouble *params);
HLAPI void HLAPIENTRY hlProxyf(HLenum pname, HLfloat param);

/* Transforms */
HLAPI void HLAPIENTRY hlMatrixMode (HLenum mode);
HLAPI void HLAPIENTRY hlPushMatrix (void);
HLAPI void HLAPIENTRY hlPopMatrix (void);
HLAPI void HLAPIENTRY hlTranslated (HLdouble x, HLdouble y, HLdouble z);
HLAPI void HLAPIENTRY hlTranslatef (HLfloat x, HLfloat y, HLfloat z);
HLAPI void HLAPIENTRY hlRotated (HLdouble angle, HLdouble x, HLdouble y, HLdouble z);
HLAPI void HLAPIENTRY hlRotatef (HLfloat angle, HLfloat x, HLfloat y, HLfloat z);
HLAPI void HLAPIENTRY hlScaled (HLdouble x, HLdouble y, HLdouble z);
HLAPI void HLAPIENTRY hlScalef (HLfloat x, HLfloat y, HLfloat z);
HLAPI void HLAPIENTRY hlLoadIdentity (void);
HLAPI void HLAPIENTRY hlLoadMatrixd (const HLdouble *m);
HLAPI void HLAPIENTRY hlLoadMatrixf (const HLfloat *m);
HLAPI void HLAPIENTRY hlMultMatrixd (const HLdouble *m);
HLAPI void HLAPIENTRY hlMultMatrixf (const HLfloat *m);
HLAPI void HLAPIENTRY hlOrtho (HLdouble left, HLdouble right, 
                               HLdouble bottom, HLdouble top, 
                               HLdouble zNear, HLdouble zFar); 
HLAPI void HLAPIENTRY hlWorkspace (HLdouble left, HLdouble bottom, HLdouble back,
                                   HLdouble right, HLdouble top, HLdouble front);

/* Local features */
HLAPI void HLAPIENTRY hlLocalFeature1fv (HLgeom *geom, HLenum type, const HLfloat *v);
HLAPI void HLAPIENTRY hlLocalFeature1dv (HLgeom *geom, HLenum type, const HLdouble *v);
HLAPI void HLAPIENTRY hlLocalFeature2fv (HLgeom *geom, HLenum type, const HLfloat *v1, const HLfloat *v2);
HLAPI void HLAPIENTRY hlLocalFeature2dv (HLgeom *geom, HLenum type, const HLdouble *v1, const HLdouble *v2);

/* Custom shape and effect callback */
HLAPI void HLAPIENTRY hlCallback (HLenum type, HLcallbackProc fn, void *userdata);

/* Event callbacks */
HLAPI void HLAPIENTRY hlAddEventCallback (HLenum event, HLuint object,
                                          HLenum thread, HLeventProc fn, 
                                          void *userdata);
HLAPI void HLAPIENTRY hlRemoveEventCallback (HLenum event, HLuint object,
                                             HLenum thread, HLeventProc fn);
HLAPI void HLAPIENTRY hlCheckEvents (void);
HLAPI void HLAPIENTRY hlEventd (HLenum pname, HLdouble param);

/* Calibration */
HLAPI void HLAPIENTRY hlUpdateCalibration (void);

/* Licensing */
HLAPI HLboolean HLAPIENTRY hlDeploymentLicense(const char* vendorName,
                                               const char* applicationName,
                                               const char* password);

/* debugging - no official support */
HLAPI void HLAPIENTRY hlDumpScene (const char* filename);

HLAPI void HLAPIENTRY hlBegin(HLenum mode);
HLAPI void HLAPIENTRY hlEnd();
HLAPI void HLAPIENTRY hlVertex3f(HLfloat x, HLfloat y, HLfloat z);
HLAPI void HLAPIENTRY hlVertex3fv(const HLfloat *params);
HLAPI void HLAPIENTRY hlVertex3d(HLdouble x, HLdouble y, HLdouble z);
HLAPI void HLAPIENTRY hlVertex3dv(const HLdouble *params);

HLAPI void HLAPIENTRY hlFrontFace(HLenum face);

#ifdef __cplusplus
}
#endif

#endif  /* HL_H_DEFINE */
/******************************************************************************/
