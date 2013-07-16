/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    hlTypes.h

Description: 

    Type definitions for the HL component.

******************************************************************************/

#ifndef HL_TYPES_H_DEFINE
#define HL_TYPES_H_DEFINE

#include <HL/hlExport.h>
#include <HL/hlCompilerConfig.h>

#include <HD/hdDefines.h>

#ifdef __cplusplus
extern "C" 
{
#endif

#if defined(WIN32) || defined(linux)
    typedef unsigned char HLboolean;
#elif defined(__APPLE__)
    typedef unsigned int HLboolean;
#endif

typedef const char* HLenum;
typedef unsigned int HLbitfield;
typedef signed char HLbyte;
typedef short HLshort;
typedef int HLint;
typedef int HLsizei;
typedef unsigned char HLubyte;
typedef unsigned short HLushort;
typedef unsigned long HLulong;
typedef unsigned int HLuint;
typedef unsigned long HLid;
typedef float HLfloat;
typedef float HLclampf;
typedef double HLdouble;
typedef double HLclampd;
typedef void HLvoid;

typedef struct
{
    HLenum errorCode;
    HDErrorInfo errorInfo;    
} HLerror;

#define HL_ERROR(X) ((X).errorCode != HL_NO_ERROR)

/* Boolean */
#define HL_TRUE                           1
#define HL_FALSE                          0

typedef void* HHLRC;
typedef void HLcache;
typedef void HLgeom;
typedef void (HLCALLBACK *HLcallbackProc)();

/* Event Callback */
typedef void (HLCALLBACK *HLeventProc) (HLenum event, HLuint object, HLenum thread,
                                        HLcache *cache, void *userdata);

/* Custom Shape Callback */
typedef HLboolean (HLCALLBACK *HLintersectLSProc) (const HLdouble startPt[3], 
                                                   const HLdouble endPt[3],
                                                   HLdouble intersectionPt[3], 
                                                   HLdouble intersectionNormal[3],
                                                   HLenum *face,
                                                   void *userdata);

typedef HLboolean (HLCALLBACK *HLclosestFeaturesProc) (const HLdouble queryPt[3], 
                                                       const HLdouble targetPt[3],
                                                       HLgeom *geom,
                                                       HLdouble closestPt[3],
                                                       void *userdata);

/* Custom Effect Callback */
typedef void (HLCALLBACK *HLcomputeForceProc) (HLdouble force[3], 
                                               HLcache *cache,
                                               void *userdata);

typedef void (HLCALLBACK *HLstartForceProc) (HLcache *cache, void *userdata);

typedef void (HLCALLBACK *HLstopForceProc) (HLcache *cache, void *userdata);


#ifdef __cplusplus
}
#endif

#endif  /* HL_TYPES_H_DEFINE */
/******************************************************************************/
