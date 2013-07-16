/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    HDDevice.h

Description: 

    Main API routines for low-level haptic device.

******************************************************************************/

#ifndef HD_DEVICE_H_DEFINE
#define HD_DEVICE_H_DEFINE

#include <HD/hdExport.h>
#include <HD/hdCompilerConfig.h>
#include <HD/hdDefines.h>

#ifdef __cplusplus
extern "C" {
#endif
    
/* Initialize the device.  Must be called before any use of the device.  
   Makes the device current. */
HDAPI HHD HDAPIENTRY hdInitDevice(HDstring pConfigName);

/* Makes the specified device current: all future calls will be performed on
   this device.  Requires a valid device id from an initialized device. */
HDAPI void HDAPIENTRY hdMakeCurrentDevice(HHD hHD);

/* Disables the specified device. */
HDAPI void HDAPIENTRY hdDisableDevice(HHD hHD);

/* Returns the ID of the current device. */
HDAPI HHD HDAPIENTRY hdGetCurrentDevice();

/* Begin grabs state information from the device. 
   End sends information such as forces to the device.
   All hdSet and state calls should be done within a begin/end frame. */ 
HDAPI void HDAPIENTRY hdBeginFrame(HHD hHD);
HDAPI void HDAPIENTRY hdEndFrame(HHD hHD);

/* Returns error information in the reverse order (i.e. most recent error
   first).  Returns an error with code HD_SUCCESS if no error on stack. */
HDAPI HDErrorInfo HDAPIENTRY hdGetError();
/* Shorthand for querying whether hdGetError reported an error. */
#define HD_DEVICE_ERROR(X) (((X).errorCode) != HD_SUCCESS)
/* Returns a readable description of the error code. */
HDAPI HDstring HDAPIENTRY hdGetErrorString(HDerror errorCode);

/* Routines for enabling or disabling a capability, or
   checking whether that capability is enabled. */
HDAPI void HDAPIENTRY hdEnable(HDenum cap);
HDAPI void HDAPIENTRY hdDisable(HDenum cap);
HDAPI HDboolean HDAPIENTRY hdIsEnabled(HDenum cap);

/* Reads the option into params array.  User is responsible for allocating 
   params of correct size for the call. */
HDAPI void HDAPIENTRY hdGetBooleanv(HDenum pname, HDboolean *params);
HDAPI void HDAPIENTRY hdGetIntegerv(HDenum pname, HDint *params);
HDAPI void HDAPIENTRY hdGetFloatv(HDenum pname, HDfloat *params);
HDAPI void HDAPIENTRY hdGetDoublev(HDenum pname, HDdouble *params);
HDAPI void HDAPIENTRY hdGetLongv(HDenum pname, HDlong *params);
HDAPI HDstring HDAPIENTRY hdGetString(HDenum pname);

/* Writes the option with the params array.  User is responsible for supplying the 
   correct number of params.  Not all types are supported for each param. */
HDAPI void HDAPIENTRY hdSetBooleanv(HDenum pname, const HDboolean *params);
HDAPI void HDAPIENTRY hdSetIntegerv(HDenum pname, const HDint *params);
HDAPI void HDAPIENTRY hdSetFloatv(HDenum pname, const HDfloat *params);
HDAPI void HDAPIENTRY hdSetDoublev(HDenum pname, const HDdouble *params);
HDAPI void HDAPIENTRY hdSetLongv(HDenum pname, const HDlong *params);

/* Calibration routines. */ 
HDAPI HDenum HDAPIENTRY hdCheckCalibration();
HDAPI HDenum HDAPIENTRY hdCheckCalibrationStyle();
HDAPI void HDAPIENTRY hdUpdateCalibrationMessage(HDenum style);
HDAPI void HDAPIENTRY hdUpdateCalibration(HDenum style);

/* Gimbal Scaling Routine */
HDAPI void HDAPIENTRY hdScaleGimbalAngles(HDdouble scaleX, HDdouble scaleY, HDdouble scaleZ, HDdouble nT[16]);


/* licensing routines */
HDAPI HDboolean HDAPIENTRY hdDeploymentLicense(
    const char* vendorName,
    const char* applicationName,
    const char* password);

#ifdef __cplusplus
}
#endif

#endif  /* HD_DEVICE_H_DEFINE */

/******************************************************************************/
