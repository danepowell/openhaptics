/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  Calibration.c

Description: 

  This example demonstrates how to handle haptic device calibration using the
  functions available in the HD API.

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <stdio.h>
#include <assert.h>

#if defined(WIN32)
# include <windows.h>
# include <conio.h>
#else
# include "conio.h"
# include <unistd.h>
# define Sleep(x) usleep((x) * 1000)
#endif

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

HDCallbackCode HDCALLBACK UpdateCalibrationCallback(void *pUserData);
HDCallbackCode HDCALLBACK CalibrationStatusCallback(void *pUserData);
HDCallbackCode HDCALLBACK DevicePositionCallback(void *pUserData);

HDenum GetCalibrationStatus();
HDboolean CheckCalibration(HDenum calibrationStyle);
void PrintDevicePosition();

/*******************************************************************************
 Main function.
*******************************************************************************/
int main(int argc, char* argv[])
{
    HHD hHD;
    HDErrorInfo error;
    int supportedCalibrationStyles;
    int calibrationStyle;
    
    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError())) 
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    printf("Calibration\n");
    printf("Found %s.\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));

    /* Choose a calibration style.  Some devices may support multiple types of 
       calibration.  In that case, prefer auto calibration over inkwell 
       calibration, and prefer inkwell calibration over reset encoders. */
    hdGetIntegerv(HD_CALIBRATION_STYLE, &supportedCalibrationStyles);
    if (supportedCalibrationStyles & HD_CALIBRATION_ENCODER_RESET)
    {
        calibrationStyle = HD_CALIBRATION_ENCODER_RESET;
    }
    if (supportedCalibrationStyles & HD_CALIBRATION_INKWELL)
    {
        calibrationStyle = HD_CALIBRATION_INKWELL;
    }
    if (supportedCalibrationStyles & HD_CALIBRATION_AUTO)
    {
        calibrationStyle = HD_CALIBRATION_AUTO;
    }

    /* Some haptic devices only support manual encoder calibration via a
       hardware reset. This requires that the endpoint be placed at a known
       physical location when the reset is commanded. For the PHANTOM haptic
       devices, this means positioning the device so that all links are
       orthogonal. Also, this reset is typically performed before the servoloop
       is running, and only technically needs to be performed once after each
       time the device is plugged in. */
    if (calibrationStyle == HD_CALIBRATION_ENCODER_RESET)
    {
        printf("Please prepare for manual calibration by\n");
        printf("placing the device at its reset position.\n\n");
        printf("Press any key to continue...\n");

        getch();

        hdUpdateCalibration(calibrationStyle);
        if (hdCheckCalibration() == HD_CALIBRATION_OK)
        {
            printf("Calibration complete.\n\n");
        }
        if (HD_DEVICE_ERROR(error = hdGetError()))
        {
            hduPrintError(stderr, &error, "Reset encoders reset failed.");
            return -1;           
        }
    }

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start the scheduler");
        return -1;           
    }

    /* Some haptic devices are calibrated when the gimbal is placed into
       the device inkwell and updateCalibration is called.  This form of
       calibration is always performed after the servoloop has started 
       running. */
    if (calibrationStyle  == HD_CALIBRATION_INKWELL)
    {
        if (GetCalibrationStatus() == HD_CALIBRATION_NEEDS_MANUAL_INPUT)
        {
            printf("Please place the device into the inkwell ");
            printf("for calibration.\n\n");
        }
    }

    printf("Press any key to quit.\n\n");

    /* Loop until key press. */
    while (!_kbhit())
    {
        /* Regular calibration should be checked periodically while the
           servoloop is running. In some cases, like the PHANTOM Desktop,
           calibration can be continually refined as the device is moved
           throughout its workspace.  For other devices that require inkwell
           reset, such as the PHANToM Omni, calibration is successfully
           performed whenever the device is put into the inkwell. */
        if (CheckCalibration(calibrationStyle))
        {
            PrintDevicePosition();
        }

        Sleep(1000);
    }
    
    hdStopScheduler();
    hdDisableDevice(hHD);

    return 0;
}

/******************************************************************************
 Begin Scheduler callbacks
 */

HDCallbackCode HDCALLBACK UpdateCalibrationCallback(void *pUserData)
{
    HDenum *calibrationStyle = (int *) pUserData;

    if (hdCheckCalibration() == HD_CALIBRATION_NEEDS_UPDATE)
    {
        hdUpdateCalibration(*calibrationStyle);
    }

    return HD_CALLBACK_DONE;
}

HDCallbackCode HDCALLBACK CalibrationStatusCallback(void *pUserData)
{
    HDenum *pStatus = (HDenum *) pUserData;

    hdBeginFrame(hdGetCurrentDevice());
    *pStatus = hdCheckCalibration();
    hdEndFrame(hdGetCurrentDevice());

    return HD_CALLBACK_DONE;
}

HDCallbackCode HDCALLBACK DevicePositionCallback(void *pUserData)
{
    HDdouble *pPosition = (HDdouble *) pUserData;

    hdBeginFrame(hdGetCurrentDevice());
    hdGetDoublev(HD_CURRENT_POSITION, pPosition);
    hdEndFrame(hdGetCurrentDevice());

    return HD_CALLBACK_DONE;
}

HDenum GetCalibrationStatus()
{
    HDenum status;
    hdScheduleSynchronous(CalibrationStatusCallback, &status,
                          HD_DEFAULT_SCHEDULER_PRIORITY);
    return status;
}

HDboolean CheckCalibration(HDenum calibrationStyle)
{
    HDenum status = GetCalibrationStatus();
    
    if (status == HD_CALIBRATION_OK)
    {
        return HD_TRUE;
    }
    else if (status == HD_CALIBRATION_NEEDS_MANUAL_INPUT)
    {
        printf("Calibration requires manual input...\n");
        return HD_FALSE;
    }
    else if (status == HD_CALIBRATION_NEEDS_UPDATE)
    {
        hdScheduleSynchronous(UpdateCalibrationCallback, &calibrationStyle,
            HD_DEFAULT_SCHEDULER_PRIORITY);

        if (HD_DEVICE_ERROR(hdGetError()))
        {
            printf("\nFailed to update calibration.\n\n");
            return HD_FALSE;
        }
        else
        {
            printf("\nCalibration updated successfully.\n\n");
            return HD_TRUE;
        }
    }
    else
    {
        assert(!"Unknown calibration status");
        return HD_FALSE;
    }
}

void PrintDevicePosition()
{
    hduVector3Dd position;

    hdScheduleSynchronous(DevicePositionCallback, position,
        HD_DEFAULT_SCHEDULER_PRIORITY);
        
    printf("Device position: %.3f %.3f %.3f\n", 
        position[0], position[1], position[2]);
}

/*
 End Scheduler callbacks
 *****************************************************************************/

/*****************************************************************************/
