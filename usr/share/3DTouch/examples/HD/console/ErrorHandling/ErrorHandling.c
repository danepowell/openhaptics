/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ErrorHandling.c

Description: 

  This example demonstrates proper error handling with HDAPI.  Device errors
  are typically recovered automatically by HDAPI.  However, it is still
  important to be notified about the error so that the haptic simulation state
  can be reset.  For instance, this example renders a fixed horizontal plane,
  but will reset contact state whenever an error occurs. T his example also
  demonstrates a few other errors types that are typical.    

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if defined(WIN32)
# include <windows.h>
# include <conio.h>
#else
# include "conio.h"
# include <unistd.h>
# include <ctype.h>
# define Sleep(x) usleep((x) * 1000)
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

/* Define some standard priorities to use when scheduling callbacks. */
#define BEGIN_FRAME_PRIORITY    HD_MAX_SCHEDULER_PRIORITY
#define END_FRAME_PRIORITY      HD_MIN_SCHEDULER_PRIORITY
#define DEFAULT_PRIORITY        HD_DEFAULT_SCHEDULER_PRIORITY

HDCallbackCode HDCALLBACK BeginFrameSchedulerCallback(void *pUserData);
HDCallbackCode HDCALLBACK EndFrameSchedulerCallback(void *pUserData);
HDCallbackCode HDCALLBACK SimulateKickCallback(void *pUserData);
HDCallbackCode HDCALLBACK SimulateTimeoutCallback(void *pUserData);

HDCallbackCode HDCALLBACK CopyLastHDErrorCallback(void *pUserData);
HDErrorInfo GetLastHDError();

void PrintHelp();

void SimulateKick();
void SimulateTimeout();
void SimulatePlaneContact();
void mainLoop();

HDErrorInfo gLastError;

/******************************************************************************
 Main function.
 Runs some scheduler callbacks and demonstrates proper handling in response
 to some common device errors.
******************************************************************************/
int main(int argc, char* argv[])
{  
    HDErrorInfo error;
    HDSchedulerHandle hBeginCallback = HD_INVALID_HANDLE;
    HDSchedulerHandle hEndCallback = HD_INVALID_HANDLE;

    /* Initialize the default haptic device. */
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    /* Schedule the begin/end pair of callbacks for bracketing the frame. */
    hBeginCallback = hdScheduleAsynchronous( 
        BeginFrameSchedulerCallback, 0, BEGIN_FRAME_PRIORITY);
    hEndCallback = hdScheduleAsynchronous( 
        EndFrameSchedulerCallback, 0, END_FRAME_PRIORITY);

    /* Modify some of the safety features, which will make it easier to
       trigger device errors. */
    hdDisable(HD_MAX_FORCE_CLAMPING);    
    hdDisable(HD_SOFTWARE_FORCE_LIMIT);
    hdEnable(HD_FORCE_OUTPUT);
    
    /* Initialize our last error before starting the scheduler. After this
       point, the servo loop will own the gLastError variable. */
    gLastError = hdGetError();

    /* Start the scheduler for interfacing with the haptic device. */
    hdStartScheduler();

    PrintHelp();
    
    /* Run the main loop, which handles keypresses. */
    mainLoop();

    /* Cleanup and shutdown the haptic device. */
    hdStopScheduler();
    hdUnschedule(hBeginCallback);
    hdUnschedule(hEndCallback);
    hdDisableDevice(hHD);

    return 0;
}

/******************************************************************************
 Main loop.
 Detects and interprets keypresses.  Monitors and initiates error recovery 
 if necessary.
******************************************************************************/
void mainLoop()
{
    HDErrorInfo error;
    int keypress;

    while (HD_TRUE)
    {
        if (_kbhit())
        {
            keypress = getch();
            keypress = toupper(keypress);
            
            switch (keypress)
            {
                case 'K': SimulateKick(); PrintHelp(); break;
                case 'T': SimulateTimeout(); PrintHelp(); break;
                case 'H': PrintHelp(); break;
                case 'Q': return;
                default: PrintHelp(); break;
            }
        }

        /* Check the last error reported from HDAPI and display the error.  Also
           turn off force output while HDAPI is reporting the warm motors case.
           The other option is to prevent the warm motors error in the first
           place by monitoring the motor temperature and adjusting force 
           output. */
        if (HD_DEVICE_ERROR(error = GetLastHDError()))
        {
            hduPrintError(stderr, &error, "Error during main loop\n"); 

            if (error.errorCode == HD_WARM_MOTORS &&
                hdIsEnabled(HD_FORCE_OUTPUT))
            {
                fprintf(stderr, "\nTurning forces off till motors cool down...\n");
                hdDisable(HD_FORCE_OUTPUT);
            }

            if (hduIsSchedulerError(&error))
            {
                /* Just quit, since the user likely unplugged the device */
                fprintf(stderr, "A scheduler error has been detected.\n");
                fprintf(stderr, "\nPress any key to quit\n");
                getch();
                return;
            }
        }
        else if (!hdIsEnabled(HD_FORCE_OUTPUT))
        {
            fprintf(stderr, "\nTurning forces back on...\n");
            hdEnable(HD_FORCE_OUTPUT);   
        }
    }
}

/******************************************************************************
 Displays a help menu describing the keyboard options
******************************************************************************/
void PrintHelp()
{
    static const char help[] = {"\
ErrorHandling Help\n\
---\n\
K: Simulate a device kick, which will cause a force or velocity error.\n\
T: Simulate a timeout, which will cause a device fault.\n\
H: Prints help menu\n\
Q: Quits the program\n\
---"};
    
    printf("\n%s\n", help);
}

/******************************************************************************
 Makes a thread-safe copy of the gLastError variable.
 *****************************************************************************/
HDCallbackCode HDCALLBACK CopyLastHDErrorCallback(void *pUserData)
{
    HDErrorInfo *pError = (HDErrorInfo *) pUserData;
    *pError = gLastError;

    gLastError = hdGetError();

    return HD_CALLBACK_DONE;
}

/******************************************************************************
 Makes a synchronous call into the servo loop to grab a copy of the last 
 error.
 *****************************************************************************/
HDErrorInfo GetLastHDError()
{
    HDErrorInfo error;
    hdScheduleSynchronous(CopyLastHDErrorCallback, &error, DEFAULT_PRIORITY);
    return error;
}

/******************************************************************************
 The first scheduler callback to be called in the loop.  Initiates the haptic 
 frame by updating device state, simulates contact with a plane.
******************************************************************************/
HDCallbackCode HDCALLBACK BeginFrameSchedulerCallback(void *pUserData)
{   
    HDErrorInfo error;

    hdBeginFrame(hdGetCurrentDevice());

    /* Always attempt to simulate contact with a plane. */
    SimulatePlaneContact();

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        gLastError = error;
    }

    return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
 The last scheduler callback to be called in the loop.  Ends
 the frame, which commits the current force for this frame.
******************************************************************************/
HDCallbackCode HDCALLBACK EndFrameSchedulerCallback(void *pUserData)
{
    HDErrorInfo error;

    hdEndFrame(hdGetCurrentDevice());

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        gLastError = error;
    }

    return HD_CALLBACK_CONTINUE;
}

/*******************************************************************************
 Simulates a horizontal one-sided infinite plane.
*******************************************************************************/
void SimulatePlaneContact()
{
    static HDboolean bAllowContact = HD_FALSE;
    hduVector3Dd planeForce, totalForce;
    hduVector3Dd position, lastPosition;
    HDdouble kStiffness;

    hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &kStiffness);    

    hdGetDoublev(HD_CURRENT_POSITION, position);
    hdGetDoublev(HD_LAST_POSITION, lastPosition);

    /* Only allow contact if the device is approaching the from the free 
       space side plane. */
    if (lastPosition[1] > 0)
    {
        bAllowContact = HD_TRUE;
    }

    /* Check if the device is in contact with the plane, and render a force. */
    if (bAllowContact && position[1] < 0)
    {
        planeForce[0] = 0;
        planeForce[1] = kStiffness * (0 - position[1]);
        planeForce[2] = 0;

        hdGetDoublev(HD_CURRENT_FORCE, totalForce);
        hduVecAdd(totalForce, totalForce, planeForce);
        hdSetDoublev(HD_CURRENT_FORCE, totalForce);
    }

    /* If a force error has occurred, reset the allowContact flag, so that the
       user has to pull out from the plane to feel it again. */
    if (hduIsForceError(&gLastError))
    {
        bAllowContact = HD_FALSE;
    }
}

/******************************************************************************
 Creates a force kick by commanding a max force for a small interval of time.
******************************************************************************/
HDCallbackCode HDCALLBACK SimulateKickCallback(void *pUserData)
{
    HDint *pFrameCount = (HDint *) pUserData;
    HDdouble forceMagnitude;
    hduVector3Dd kickForce, totalForce;

    hdGetDoublev(HD_NOMINAL_MAX_FORCE, &forceMagnitude);
    hduVecSet(kickForce, 0, forceMagnitude, 0);

    /* Add a kick force to the current force. */
    hdGetDoublev(HD_CURRENT_FORCE, totalForce);
    hduVecAdd(totalForce, totalForce, kickForce);
    hdSetDoublev(HD_CURRENT_FORCE, totalForce);

    *pFrameCount += 1;

    if (*pFrameCount < 100)
        return HD_CALLBACK_CONTINUE;    
    else
        return HD_CALLBACK_DONE;
}


/******************************************************************************
 Schedules a callback that generates a kick.
******************************************************************************/
void SimulateKick()
{
    HDint nFrameCount = 0;

    static const char help[] = {"\
This test will simulate a kick by generating a substantial force for a small\n\
period of time. The HDAPI should detect the kick using using either its\n\
velocity or force impulse detection.\n\n\
Please stand clear of the device and proceed with caution...\n\n\
Hit any key to simulate a kick.\n"};

    printf("\n%s\n", help);
    getch();

    /* Schedule a callback to simulate a kick and wait for it to complete. */
    hdScheduleSynchronous(SimulateKickCallback, &nFrameCount, DEFAULT_PRIORITY);
}

/******************************************************************************
 Performs a sleep in the servo loop thread, which will cause the hardware
 to be disable itself, since it needs frequent updates to stay alive.
******************************************************************************/
HDCallbackCode HDCALLBACK SimulateTimeoutCallback(void *pUserData)
{
    Sleep(1000);

    return HD_CALLBACK_DONE;
}

/******************************************************************************
 Schedules a callback that generates a timeout.
******************************************************************************/
void SimulateTimeout()
{
    static const char help[] = {\
"This test simulates a timeout in the servo loop. If the duty cycle of the\n\
 servo loop runs too long, the device can become very unstable. To combat\n\
 this, the hardware typically has support for a watchdog that ensures that\n\
 the device is being updated at regular intervals.\n\n\
 "};

    
    printf("\n%s\n", help);

    printf("Hold the stylus against the frictionless plane and..\n");
    printf("Hit any key to simulate a timeout.\n");


    getch();
    printf("Forces should stop momentarily then reappear, please wait...\n");
    
    hdScheduleSynchronous(SimulateTimeoutCallback, 0, DEFAULT_PRIORITY);
}

/*****************************************************************************/
