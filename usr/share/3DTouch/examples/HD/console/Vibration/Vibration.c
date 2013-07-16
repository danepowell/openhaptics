/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  Vibration.c

Description: 

  This example shows how to generate a simple sinusoidal vibration effect
  for the haptic device.  The amplitude and frequency of the vibration effect
  can be adjusted.

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
# include <ctype.h>
typedef int BOOL;
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

static HDint gVibrationFreq = 100; /* Hz */
static HDdouble gVibrationAmplitude = 0; /* N */

HDSchedulerHandle gCallbackHandle = 0;

void printHelp();
void mainLoop();
BOOL initDemo();

HDCallbackCode HDCALLBACK SetVibrationFreqCallback(void *pUserData);
HDCallbackCode HDCALLBACK SetVibrationAmplitudeCallback(void *pUserData);
HDCallbackCode HDCALLBACK VibrationCallback(void *pUserData);

/******************************************************************************
 Initiates a vibration effect that can be controlled using a keyboard menu
 from the main loop.
******************************************************************************/
int main(int argc, char* argv[])
{  
    HDErrorInfo error;
    
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    printf("Vibration Effect Example\n");

    gCallbackHandle = hdScheduleAsynchronous(
        VibrationCallback, 0, HD_MAX_SCHEDULER_PRIORITY);

    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    mainLoop();

    hdStopScheduler();
    hdUnschedule(gCallbackHandle);
    hdDisableDevice(hHD);

    return 0;
}

void printHelp()
{
    static const char help[] = {"\
-----------------------------------------------\n\
      Vibration Example Menu Options\n\
===============================================\n\
[+] : Increase vibration frequency\n\
[-] : Decrease vibration frequency\n\
[[] : Decrease vibration force\n\
[]] : Increase vibration force\n\
[H] : Print help menu\n\
[Q] : Quit"};

    printf("%s\n\n", help);
}



/******************************************************************************
 Runs the main loop of the application.  Queries for keypresses to control
 the frequency of the vibration effect.
******************************************************************************/
void mainLoop()
{
    HDint nFrequency = gVibrationFreq;
    HDdouble amplitude;
    HDdouble maxAmplitude;
    int keypress;
    
    if (!initDemo())
    {
        printf("Demo Initialization failed\n");
        printf("Press any key to exit\n");
        getch();

    }

    /* Start the amplitude at a fraction of the max continuous 
       force value.  Remember the max continuous force value to be
       used as the max amplitude. This cap prevents the user
       from increasing the amplitude to dangerous limits. */
    hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &amplitude);
    maxAmplitude = amplitude;
    amplitude *= 0.75;
    gVibrationAmplitude = amplitude;

    printHelp();

    while (HD_TRUE)
    {
        if (_kbhit())
        {
            keypress = toupper(getch());

            if (keypress == 'Q')
            {
                break;
            }
            else if (keypress == '+' || keypress == '=')
            {
                ++nFrequency;
                if (nFrequency > 500)
                    nFrequency = 500;
                
                hdScheduleSynchronous(SetVibrationFreqCallback, &nFrequency,
                                      HD_DEFAULT_SCHEDULER_PRIORITY);
            }
            else if (keypress == '-' || keypress == '_')
            {
                --nFrequency;
                if (nFrequency < 0)
                    nFrequency = 0;
                
                hdScheduleSynchronous(SetVibrationFreqCallback, &nFrequency,
                                      HD_DEFAULT_SCHEDULER_PRIORITY);
            }
            else if (keypress == '[' || keypress == '{')
            {
                amplitude -= .1;
                if (amplitude < 0)
                    amplitude = 0;
                hdScheduleSynchronous(SetVibrationAmplitudeCallback, &amplitude,
                                      HD_DEFAULT_SCHEDULER_PRIORITY);
            }
            else if (keypress == ']' || keypress == '}')
            {
                amplitude += .1;
                if (amplitude > maxAmplitude)
                    amplitude = maxAmplitude;
                hdScheduleSynchronous(SetVibrationAmplitudeCallback, &amplitude,
                                      HD_DEFAULT_SCHEDULER_PRIORITY);
            }
            else
            {
                printHelp();
            }

            printf("Vibration Frequency: %d\n", nFrequency);
            printf("Vibration Amplitude: %lf\n", amplitude);
            printf("\n");
        }
        
        /* Check if the main scheduler callback has exited. */
        if (!hdWaitForCompletion(gCallbackHandle, HD_WAIT_CHECK_STATUS))
        {
            fprintf(stderr, "\nThe main scheduler callback has exited\n");
            fprintf(stderr, "\nPress any key to quit.\n");
            getch();
            return;
        }
    }
}
/******************************************************************************
 Initlaizes the demo by preparing the user to hold the device..
******************************************************************************/
BOOL initDemo(void)
{
    HDErrorInfo error;
    int calibrationStyle;
    printf("Demo Instructions\n");

    hdGetIntegerv(HD_CALIBRATION_STYLE, &calibrationStyle);
    if (calibrationStyle & HD_CALIBRATION_AUTO || calibrationStyle & HD_CALIBRATION_INKWELL)
    {
        printf("Please prepare for starting the demo by \n");
        printf("holding the device handle firmly and\n\n");
        printf("Press any key to continue...\n");
        getch();
        return 1;
    }
    if (calibrationStyle & HD_CALIBRATION_ENCODER_RESET )
    {
        printf("Please prepare for starting the demo by \n");
        printf("holding the device handle firmly and \n\n");
        printf("Press any key to continue...\n");

        getch();

        hdUpdateCalibration(calibrationStyle);
        if (hdCheckCalibration() == HD_CALIBRATION_OK)
        {
            printf("Calibration complete.\n\n");
            return 1;
        }
        if (HD_DEVICE_ERROR(error = hdGetError()))
        {
            hduPrintError(stderr, &error, "Reset encoders reset failed.");
            return 0;           
        }
    }
}

/******************************************************************************
 Simulates a vibration using a sinusoidal wave along the Y axis.
******************************************************************************/
HDCallbackCode HDCALLBACK VibrationCallback(void *pUserData)
{
    static const hduVector3Dd direction = { 0, 1, 0 };
    HDErrorInfo error;
    hduVector3Dd force;
    HDdouble instRate;
    static HDdouble timer = 0;

    hdBeginFrame(hdGetCurrentDevice());

    /* Use the reciprocal of the instantaneous rate as a timer. */
    hdGetDoublev(HD_INSTANTANEOUS_UPDATE_RATE, &instRate);
    timer += 1.0 / instRate;

    /* Apply a sinusoidal force in the direction of motion. */
    hduVecScale(force, direction, gVibrationAmplitude * sin(timer * gVibrationFreq));
    
    hdSetDoublev(HD_CURRENT_FORCE, force);

    hdEndFrame(hdGetCurrentDevice());

    /* Check if an error occurred while attempting to render the force. */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, 
                      "Error detected during scheduler callback.\n");

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
 Modifies the vibration frequency being used by the haptic thread.
******************************************************************************/
HDCallbackCode HDCALLBACK SetVibrationFreqCallback(void *pUserData)
{
    HDint *nFrequency = (HDint *) pUserData;
        
    gVibrationFreq = *nFrequency;        

    return HD_CALLBACK_DONE;
}

/******************************************************************************
 Modifies the vibration amplitude being used by the haptic thread.
******************************************************************************/
HDCallbackCode HDCALLBACK SetVibrationAmplitudeCallback(void *pUserData)
{
    HDdouble *amplitude = (HDdouble *) pUserData;
        
    gVibrationAmplitude = *amplitude;        

    return HD_CALLBACK_DONE;
}

/*****************************************************************************/
