/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  PreventWarmMotors.c

Description: 

  This example demonstrates a simple approach to using the normalized motor
  temperature and continuous max force to prevent the motors of the haptic
  device from overheating.

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#if defined(WIN32)
# include <windows.h>
# include <conio.h>
#else
# include "conio.h"
# include <string.h>
# include <unistd.h>
# define Sleep(x) usleep((x) * 1000)
# define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include <HD/hd.h>
#include <HDU/hduVector.h>
#include <HDU/hduError.h>

static HDdouble *gServoMotorTemp = 0;
static HDint gNumMotors;

static volatile HDboolean bRampDownForces = HD_FALSE;
static volatile HDboolean bRampUpForces = HD_FALSE;

/* Callback Function prototypes. */
HDCallbackCode HDCALLBACK QueryMotorTemp(void *pUserData);
HDCallbackCode HDCALLBACK ServoSchedulerCallback(void *pUserData);

/* Helper Function prototypes. */
void PrintMotorTemp(const HDdouble *aMotorTemp, HDint nNumMotors);
void PreventWarmMotors(hduVector3Dd force);

/*******************************************************************************
 Main function
 Handles initialization, setup of callbacks, and shutdown.
*******************************************************************************/
int main(int argc, char *argv[])
{    
    HDErrorInfo error;
    HDSchedulerHandle hServoCallback;
    HDdouble *aMotorTemp = 0;

    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError())) 
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    printf("Handling Warm Motors Example\n");

    /* Query the number of output DOF (i.e. num motors). */
    hdGetIntegerv(HD_OUTPUT_DOF, &gNumMotors);
    aMotorTemp = (HDdouble *) malloc(sizeof(HDdouble) * gNumMotors);
    gServoMotorTemp = (HDdouble *) malloc(sizeof(HDdouble) * gNumMotors);

    hdEnable(HD_FORCE_OUTPUT);
    hdEnable(HD_FORCE_RAMPING);
    hdEnable(HD_MAX_FORCE_CLAMPING);

    /* Schedule one callback for refreshing the device. */
    hServoCallback = hdScheduleAsynchronous(
        ServoSchedulerCallback, 0, HD_MAX_SCHEDULER_PRIORITY);

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        return -1;
    }
    
    /* Loop until a key is pressed. */
    printf("Press any key to quit.\n\n");
    while (!_kbhit())
    {
        Sleep(500);

        hdScheduleSynchronous(QueryMotorTemp, aMotorTemp,
                              HD_DEFAULT_SCHEDULER_PRIORITY);
        
        PrintMotorTemp(aMotorTemp, gNumMotors);

        /* Periodically check if the scheduler callback has exited. */
        if (!hdWaitForCompletion(hServoCallback, HD_WAIT_CHECK_STATUS))
        {
            fprintf(stderr, "Press any key to quit.\n");     
            getch();
            break;
        }
    }

    /* Cleanup the scheduler and disable the device instance. */
    hdStopScheduler();
    hdUnschedule(hServoCallback);
    hdDisableDevice(hHD);

    free(gServoMotorTemp);
    free(aMotorTemp);

    return 0;
}

/*******************************************************************************
 Callback that queries motor temperature.
*******************************************************************************/
HDCallbackCode HDCALLBACK QueryMotorTemp(void *pUserData)
{
    HDdouble *aMotorTemp = (HDdouble *) pUserData;

    hdGetDoublev(HD_MOTOR_TEMPERATURE, aMotorTemp);

    return HD_CALLBACK_DONE;
}

/*******************************************************************************
 Callback that sets a force.
*******************************************************************************/
HDCallbackCode HDCALLBACK ServoSchedulerCallback(void *pUserData)
{
    HDErrorInfo error;
    hduVector3Dd position;
    hduVector3Dd force;
    HDdouble kStiffness;

    hdBeginFrame(hdGetCurrentDevice());

    /* Render a simple horizontal plane. */
    hdGetDoublev(HD_CURRENT_POSITION, position);
    hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &kStiffness);

    memset(force, 0, sizeof(force));
    if (position[1] < 0)
    {
        force[1] = kStiffness * (0 - position[1]);
    }    

    PreventWarmMotors(force);    

    hdSetDoublev(HD_CURRENT_FORCE, force);

    hdEndFrame(hdGetCurrentDevice());

    /* Check if an error occurred while attempting to render the force */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error,
            "Error detected during main scheduler callback\n");

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    
    return HD_CALLBACK_CONTINUE;
}

/*******************************************************************************
 Print motor temperature utility.
*******************************************************************************/
void PrintMotorTemp(const HDdouble *aMotorTemp, HDint nNumMotors)
{
    HDint i;

    printf("Motor Temperature:");
    for (i = 0; i < nNumMotors; i++)
    {
        printf(" %f", aMotorTemp[i]);
    }

    if (bRampDownForces)
    {
        printf(" Ramping Down Forces");
    }
    else if (bRampUpForces)
    {
        printf(" Ramping Up Forces");
    }

    printf("\n");
}

/*******************************************************************************
 Scales the force down, taking into account the motor temperature.
*******************************************************************************/
void PreventWarmMotors(hduVector3Dd force)
{
    static const HDdouble kRampDownTurnOn = 0.7;
    static const HDdouble kRampDownTurnOff = 0.5;
    static const HDdouble kRampDownRate = 0.999;
    static const HDdouble kRampUpRate = 1.001;
    static HDdouble fAdaptiveClamp = DBL_MAX;

    HDdouble kMaxContinuousForce;
    HDdouble fForceMag;
    HDdouble fMaxTemp = 0;
    HDint i;

    /* Determine if any of the motors are above the temperature threshold.
       All temperature values are normalized between room temperature and the
       overheat temperature. */
    hdGetDoublev(HD_MOTOR_TEMPERATURE, gServoMotorTemp);

    for (i = 0; i < gNumMotors; i++)
    {
        if (gServoMotorTemp[i] > fMaxTemp)
        {
            fMaxTemp = gServoMotorTemp[i];                
        }
    }

    fForceMag = hduVecMagnitude(force);

    if (!bRampDownForces && fMaxTemp > kRampDownTurnOn)
    {
        bRampDownForces = HD_TRUE;
        bRampUpForces = HD_FALSE;
        fAdaptiveClamp = fForceMag;
    }
    else if (bRampDownForces && fMaxTemp < kRampDownTurnOff)
    {
        bRampDownForces = HD_FALSE;
        bRampUpForces = HD_TRUE;
    }

    if (bRampDownForces)
    {
        /* Determine a force clamp magnitude that will gradually bring the force
           towards the continuous max force magnitude.  SensAble claims that the
           device can operate continuously at that force without overheating. */
        hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &kMaxContinuousForce);

        fAdaptiveClamp *= kRampDownRate;
        fAdaptiveClamp = max(fAdaptiveClamp, kMaxContinuousForce);
    }
    else if (bRampUpForces)
    {
        if (fForceMag < fAdaptiveClamp)
        {
            bRampUpForces = HD_FALSE;
            fAdaptiveClamp = DBL_MAX;
        }

        fAdaptiveClamp *= kRampUpRate;
    }

    /* The HD API presently does not provide a way to convert a cartesian
       force to motor torques, so we cannot adjust the motor torques directly
       to cool just the warm motor.  Therefore, we need to impose an overall
       clamp on the force magnitude in the cartesian domain. */
    if (fForceMag > fAdaptiveClamp)
    {
        hduVecNormalizeInPlace(force);
        hduVecScaleInPlace(force, fAdaptiveClamp);
    }
}

/*****************************************************************************/
