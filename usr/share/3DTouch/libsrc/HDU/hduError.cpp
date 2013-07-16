/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hduError.cpp

Description: 

  Helper routines for print error messages and error handling with the HD API.

*******************************************************************************/

#include "hduAfx.h"

#include <HDU/hduError.h>
#include <HD/hd.h>

namespace
{

const char *getErrorCodeName(HDerror errorCode)
{
    switch (errorCode)
    {
        case HD_SUCCESS: return "HD_SUCCESS";

        /* Function errors */
        case HD_INVALID_ENUM: return "HD_INVALID_ENUM";
        case HD_INVALID_VALUE: return "HD_INVALID_VALUE";
        case HD_INVALID_OPERATION: return "HD_INVALID_OPERATION";
        case HD_INVALID_INPUT_TYPE: return "HD_INVALID_INPUT_TYPE";
        case HD_BAD_HANDLE: return "HD_BAD_HANDLE";

        /* Force errors */
        case HD_WARM_MOTORS: return "HD_WARM_MOTORS";
        case HD_EXCEEDED_MAX_FORCE: return "HD_EXCEEDED_MAX_FORCE";
        case HD_EXCEEDED_MAX_VELOCITY: return "HD_EXCEEDED_MAX_VELOCITY";
        case HD_FORCE_ERROR: return "HD_FORCE_ERROR";

        /* Device errors */
        case HD_DEVICE_FAULT: return "HD_DEVICE_FAULT";
        case HD_DEVICE_ALREADY_INITIATED: return "HD_DEVICE_ALREADY_INITIATED";
        case HD_COMM_ERROR: return "HD_COMM_ERROR";
        case HD_COMM_CONFIG_ERROR: return "HD_COMM_CONFIG_ERROR";
        case HD_TIMER_ERROR: return "HD_TIMER_ERROR";

        /* Haptic rendering context */
        case HD_ILLEGAL_BEGIN: return "HD_ILLEGAL_BEGIN";
        case HD_ILLEGAL_END: return "HD_ILLEGAL_END";
        case HD_FRAME_ERROR: return "HD_FRAME_ERROR";

        /* Scheduler errors */
        case HD_INVALID_PRIORITY: return "HD_INVALID_PRIORITY";
        case HD_SCHEDULER_FULL: return "HD_SCHEDULER_FULL";

        /* Licensing errors */
        case HD_INVALID_LICENSE: return "HD_INVALID_LICENSE";

        default: return "Unknown Error Code";
    }
}

} /* anonymous namespace */

/******************************************************************************
 Pretty prints a message containing the error code name, the error code and
 the internal error code.
******************************************************************************/
void hduPrintError(FILE *stream, const HDErrorInfo *error,
                   const char *message)
{
    fprintf(stream, "HD Error: %s\n", getErrorCodeName(error->errorCode));
    fprintf(stream, "%s\n", hdGetErrorString(error->errorCode));
    fprintf(stream, "HHD: %X\n", error->hHD);
    fprintf(stream, "Error Code: %X\n", error->errorCode);
    fprintf(stream, "Internal Error Code: %d\n", error->internalErrorCode);
    fprintf(stream, "Message: %s\n", message);
}

/******************************************************************************
 Pretty prints a message containing the error code name, the error code and
 the internal error code.
******************************************************************************/
std::ostream &operator<<(std::ostream &os, const HDErrorInfo &error)
{    
    os << "HD Error : " << getErrorCodeName(error.errorCode) << std::endl;
    os << hdGetErrorString(error.errorCode) << std::endl;
	os.setf(std::ios::hex);
    os << "HHD: " << error.hHD << std::endl;
    os << "Error Code: " << error.errorCode << std::endl;
	os.setf(std::ios::dec);
    os << "Internal Error Code: " << error.internalErrorCode << std::endl;

    return os;    
}

/******************************************************************************
 Indicates whether this is a recoverable error related to haptic rendering.
******************************************************************************/
HDboolean hduIsForceError(const HDErrorInfo *error)
{
    switch (error->errorCode)
    {
        case HD_WARM_MOTORS:   
        case HD_EXCEEDED_MAX_FORCE:
        case HD_EXCEEDED_MAX_VELOCITY:
        case HD_FORCE_ERROR:
            return HD_TRUE;

        default:
            return HD_FALSE;
    }
}

/******************************************************************************
 Indicates whether this is a communication, timing or general scheduler error
 that would prevent proper operation of the servo loop
******************************************************************************/
HDboolean hduIsSchedulerError(const HDErrorInfo *error)
{
    switch (error->errorCode)
    {
        case HD_COMM_ERROR:
        case HD_COMM_CONFIG_ERROR:
        case HD_TIMER_ERROR:
        case HD_INVALID_PRIORITY:
        case HD_SCHEDULER_FULL:
            return HD_TRUE;

        default:
            return HD_FALSE;
    }
}
