/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

    hlDefines.h

Description: 

    Constant definitions and macros for the HL component.

*****************************************************************************/
#ifndef _STI_HL_DEFINES_H_DEFINE
#define _STI_HL_DEFINES_H_DEFINE


#include <HL/hlExport.h>
#include <HL/hlCompilerConfig.h>
#include <HL/hlTypes.h>

#ifdef __cplusplus
extern "C" 
{
#endif


/* ErrorCode */
extern HLAPI HLenum HL_NO_ERROR;
extern HLAPI HLenum HL_INVALID_ENUM;
extern HLAPI HLenum HL_INVALID_VALUE;
extern HLAPI HLenum HL_INVALID_OPERATION;
extern HLAPI HLenum HL_STACK_OVERFLOW;
extern HLAPI HLenum HL_STACK_UNDERFLOW;
extern HLAPI HLenum HL_OUT_OF_MEMORY;
extern HLAPI HLenum HL_DEVICE_ERROR;

/* Haptic Material Properties */
extern HLAPI HLenum HL_STIFFNESS;
extern HLAPI HLenum HL_DAMPING;
extern HLAPI HLenum HL_STATIC_FRICTION;
extern HLAPI HLenum HL_DYNAMIC_FRICTION;
extern HLAPI HLenum HL_TOUCHABLE_FACE;
extern HLAPI HLenum HL_POPTHROUGH;

extern HLAPI HLenum HL_FRONT;
extern HLAPI HLenum HL_BACK;
extern HLAPI HLenum HL_FRONT_AND_BACK;

/* Touch Model */
extern HLAPI HLenum HL_TOUCH_MODEL;
extern HLAPI HLenum HL_CONTACT;
extern HLAPI HLenum HL_CONSTRAINT;
extern HLAPI HLenum HL_SNAP_DISTANCE;

/* Enable */
extern HLAPI HLenum HL_PROXY_RESOLUTION;
extern HLAPI HLenum HL_HAPTIC_CAMERA_VIEW;
extern HLAPI HLenum HL_ADAPTIVE_VIEWPORT;

/* Haptic Shape Types */
extern HLAPI HLenum HL_SHAPE_FEEDBACK_BUFFER;
extern HLAPI HLenum HL_SHAPE_DEPTH_BUFFER;
extern HLAPI HLenum HL_SHAPE_CALLBACK;
extern HLAPI HLenum HL_SHAPE_PRIMITIVE;

/* Proxy State */
extern HLAPI HLenum HL_PROXY_IS_TOUCHING;
extern HLAPI HLenum HL_PROXY_TOUCH_NORMAL;

/* Shape State */
extern HLAPI HLenum HL_REACTION_FORCE;

/* Hint */
extern HLAPI HLenum HL_SHAPE_FEEDBACK_BUFFER_VERTICES;
extern HLAPI HLenum HL_SHAPE_DYNAMIC_SURFACE_CHANGE;

/* MatrixMode */
extern HLAPI HLenum HL_VIEWTOUCH;
extern HLAPI HLenum HL_TOUCHWORKSPACE;
extern HLAPI HLenum HL_MODELVIEW;

/* GL Modelview Matrix */
extern HLAPI HLenum HL_USE_GL_MODELVIEW;

/* Get Properties */
extern HLAPI HLenum HL_VIEWTOUCH_MATRIX;
extern HLAPI HLenum HL_TOUCHWORKSPACE_MATRIX;
extern HLAPI HLenum HL_MODELVIEW_MATRIX;
extern HLAPI HLenum HL_WORKSPACE;
extern HLAPI HLenum HL_MAX_WORKSPACE_DIMS;

/* Proxy and Device State 
   (in workspace coordinates) */
extern HLAPI HLenum HL_PROXY_POSITION;
extern HLAPI HLenum HL_PROXY_ROTATION;
extern HLAPI HLenum HL_PROXY_TRANSFORM;
extern HLAPI HLenum HL_DEVICE_POSITION;
extern HLAPI HLenum HL_DEVICE_ROTATION;
extern HLAPI HLenum HL_DEVICE_TRANSFORM;
extern HLAPI HLenum HL_DEVICE_FORCE;
extern HLAPI HLenum HL_DEVICE_TORQUE;
extern HLAPI HLenum HL_BUTTON1_STATE;
extern HLAPI HLenum HL_BUTTON2_STATE;
extern HLAPI HLenum HL_BUTTON3_STATE;
extern HLAPI HLenum HL_SAFETY_STATE;
extern HLAPI HLenum HL_INKWELL_STATE;
extern HLAPI HLenum HL_DEPTH_OF_PENETRATION;

/* Golden Position and Radius 
   (in world coordinates) */
extern HLAPI HLenum HL_GOLDEN_POSITION;
extern HLAPI HLenum HL_GOLDEN_RADIUS;

/* Event Types */
extern HLAPI HLenum HL_EVENT_MOTION;
extern HLAPI HLenum HL_EVENT_1BUTTONDOWN;
extern HLAPI HLenum HL_EVENT_1BUTTONUP;
extern HLAPI HLenum HL_EVENT_2BUTTONDOWN;
extern HLAPI HLenum HL_EVENT_2BUTTONUP;
extern HLAPI HLenum HL_EVENT_3BUTTONDOWN;
extern HLAPI HLenum HL_EVENT_3BUTTONUP;
extern HLAPI HLenum HL_EVENT_SAFETYDOWN;
extern HLAPI HLenum HL_EVENT_SAFETYUP;
extern HLAPI HLenum HL_EVENT_INKWELLDOWN;
extern HLAPI HLenum HL_EVENT_INKWELLUP;
extern HLAPI HLenum HL_EVENT_TOUCH;
extern HLAPI HLenum HL_EVENT_UNTOUCH;
extern HLAPI HLenum HL_EVENT_CALIBRATION_UPDATE;
extern HLAPI HLenum HL_EVENT_CALIBRATION_INPUT;

/* Tolerance for Motion Events */
extern HLAPI HLenum HL_EVENT_MOTION_LINEAR_TOLERANCE;
extern HLAPI HLenum HL_EVENT_MOTION_ANGULAR_TOLERANCE;

/* Shape/Effect Id for Events */
extern HLAPI HLint HL_OBJECT_ANY;

/* Custom Shape Callbacks */
extern HLAPI HLenum HL_SHAPE_INTERSECT_LS;
extern HLAPI HLenum HL_SHAPE_CLOSEST_FEATURES;

/* Local Features 
   (Used by custom shapes) */
extern HLAPI HLenum HL_LOCAL_FEATURE_POINT;
extern HLAPI HLenum HL_LOCAL_FEATURE_LINE;
extern HLAPI HLenum HL_LOCAL_FEATURE_PLANE;

/* Thread Identifiers */
extern HLAPI HLenum HL_CLIENT_THREAD;
extern HLAPI HLenum HL_COLLISION_THREAD;

/* Attribute Masks */
extern HLAPI HLint HL_HINT_BIT;
extern HLAPI HLint HL_MATERIAL_BIT;
extern HLAPI HLint HL_TOUCH_BIT;
extern HLAPI HLint HL_TRANSFORM_BIT;
extern HLAPI HLint HL_EFFECT_BIT;
extern HLAPI HLint HL_EVENTS_BIT;

/* Force Effect Parameters */
extern HLAPI HLenum HL_EFFECT_CALLBACK;
extern HLAPI HLenum HL_EFFECT_CONSTANT;
extern HLAPI HLenum HL_EFFECT_SPRING;
extern HLAPI HLenum HL_EFFECT_VISCOUS;
extern HLAPI HLenum HL_EFFECT_FRICTION;

extern HLAPI HLenum HL_EFFECT_PROPERTY_TYPE;
extern HLAPI HLenum HL_EFFECT_PROPERTY_GAIN;
extern HLAPI HLenum HL_EFFECT_PROPERTY_MAGNITUDE;
extern HLAPI HLenum HL_EFFECT_PROPERTY_FREQUENCY;
extern HLAPI HLenum HL_EFFECT_PROPERTY_DURATION;
extern HLAPI HLenum HL_EFFECT_PROPERTY_POSITION;
extern HLAPI HLenum HL_EFFECT_PROPERTY_DIRECTION;
extern HLAPI HLenum HL_EFFECT_PROPERTY_ACTIVE;

extern HLAPI HLenum HL_EFFECT_COMPUTE_FORCE;
extern HLAPI HLenum HL_EFFECT_START;
extern HLAPI HLenum HL_EFFECT_STOP;

/* hlGetString Parameters */
extern HLAPI HLenum HL_VERSION;
extern HLAPI HLenum HL_VENDOR;

/* Experimental / Prototype 
   (may be deprecated or removed after
    this version ) */
extern HLAPI HLenum HL_SHAPE_PERSISTENCE;

extern HLAPI HLenum HL_TRIANGLES;
extern HLAPI HLenum HL_TRIANGLE_STRIP;
extern HLAPI HLenum HL_QUADS;
extern HLAPI HLenum HL_QUAD_STRIP;
extern HLAPI HLenum HL_POLYGON;
extern HLAPI HLenum HL_LINES;
extern HLAPI HLenum HL_LINE_STRIP;
extern HLAPI HLenum HL_LINE_LOOP;
extern HLAPI HLenum HL_POINTS;
    
extern HLAPI HLenum HL_CW;
extern HLAPI HLenum HL_CCW;

#define HL_VERSION_MAJOR_NUMBER 3
#define HL_VERSION_MINOR_NUMBER 00

#ifdef __cplusplus
}
#endif

#endif  /* _STI_HL_DEFINES_H_DEFINE */
/******************************************************************************/
