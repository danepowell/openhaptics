/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  FrictionlessSphere.cpp

Description: 

  This example demonstrates how to haptically render contact with a
  frictionless sphere.

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <cstdio>
#include <cassert>

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

/*******************************************************************************
 Haptic sphere callback.  
 The sphere is oriented at 0,0,0 with radius 40, and provides a repelling force 
 if the device attempts to penetrate through it. 
*******************************************************************************/
HDCallbackCode HDCALLBACK FrictionlessSphereCallback(void *data)
{
    const double sphereRadius = 40.0;
    const hduVector3Dd spherePosition(0,0,0);

    // Stiffness, i.e. k value, of the sphere.  Higher stiffness results
    // in a harder surface.
    const double sphereStiffness = .25;

    hdBeginFrame(hdGetCurrentDevice());
   
    // Get the position of the device.
    hduVector3Dd position;
    hdGetDoublev(HD_CURRENT_POSITION, position);
    
    // Find the distance between the device and the center of the
    // sphere.
    double distance = (position-spherePosition).magnitude();

    // If the user is within the sphere -- i.e. if the distance from the user to 
    // the center of the sphere is less than the sphere radius -- then the user 
    // is penetrating the sphere and a force should be commanded to repel him 
    // towards the surface.
    if (distance < sphereRadius)
    {
        // Calculate the penetration distance.
        double penetrationDistance = sphereRadius-distance;

        // Create a unit vector in the direction of the force, this will always 
        // be outward from the center of the sphere through the user's 
        // position.
        hduVector3Dd forceDirection = (position-spherePosition)/distance;

        // Use F=kx to create a force vector that is away from the center of 
        // the sphere and proportional to the penetration distance, and scsaled 
        // by the object stiffness.  
        // Hooke's law explicitly:
        double k = sphereStiffness;
        hduVector3Dd x = penetrationDistance*forceDirection;
        hduVector3Dd f = k*x;
        hdSetDoublev(HD_CURRENT_FORCE, f);
    }

    hdEndFrame(hdGetCurrentDevice());

    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Error during main scheduler callback\n");

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }        
    }

    return HD_CALLBACK_CONTINUE;
}


/******************************************************************************
 main function
 Initializes the device, creates a callback to handle sphere forces, terminates
 upon key press.
******************************************************************************/
int main(int argc, char* argv[])
{
    HDErrorInfo error;
    // Initialize the default haptic device.
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    // Start the servo scheduler and enable forces.
    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }
        
    // Application loop - schedule our call to the main callback.
    HDSchedulerHandle hSphereCallback = hdScheduleAsynchronous(
        FrictionlessSphereCallback, 0, HD_DEFAULT_SCHEDULER_PRIORITY);

    printf("Sphere example.\n");
    printf("Move the device around to feel a frictionless sphere\n\n");
    printf("Press any key to quit.\n\n");

    while (!_kbhit())
    {
        if (!hdWaitForCompletion(hSphereCallback, HD_WAIT_CHECK_STATUS))
        {
            fprintf(stderr, "\nThe main scheduler callback has exited\n");
            fprintf(stderr, "\nPress any key to quit.\n");
            getch();
            break;
        }
    }

    // For cleanup, unschedule our callbacks and stop the servo loop.
    hdStopScheduler();
    hdUnschedule(hSphereCallback);
    hdDisableDevice(hHD);

    return 0;
}

/*****************************************************************************/

