/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  CommandMotorDAC.c

Description: 

  This example demonstrates commanding DAC values directly to the motors
  via a keyboard menu interface.

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
#include<time.h>
#include "conio.h"
#define FALSE 0
#define TRUE 1
#endif

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

/* Assumes the unit is a PHANTOM from SensAble Technologies, Inc. */
#define MAX_INPUT_DOF   6   
#define MAX_OUTPUT_DOF  6

static int gNumMotors = 0;
static int gNumEncoders = 0;

static long alMotorDACValuesApp[MAX_OUTPUT_DOF];
static long alMotorDACValuesServo[MAX_OUTPUT_DOF];

void PrintHelp()
{
    static const char help[] = {\
"CommandMotorDAC Help\n\
---\n\
M: Motor Selection\n\
V: Set DAC value\n\
P: Prints device state\n\
C: Continuously prints device state\n\
H: Prints help menu\n\
Q: Quits the program\n\
---"};
    
    printf("\n%s\n", help);
}

HDSchedulerHandle gCallbackHandle = HD_INVALID_HANDLE;

void mainLoop();

/*****************************************************************************
 Prints DAC values.
*****************************************************************************/
void PrintDACValues()
{
    int i;

    printf("Motor DAC Values:");
    for (i = 0; i < gNumMotors; i++)
    {
        printf(" %d", alMotorDACValuesApp[i]);
    }
    printf("\n\n");
}

/*****************************************************************************
 Directly sets the DAC values.
*****************************************************************************/
HDCallbackCode HDCALLBACK ServoSchedulerCallback(void *pUserData)
{
    HDErrorInfo error;

    hdBeginFrame(hdGetCurrentDevice());    
    
    assert(alMotorDACValuesServo);

    hdSetLongv(HD_CURRENT_MOTOR_DAC_VALUES, alMotorDACValuesServo);

    hdEndFrame(hdGetCurrentDevice());

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Error while commanding DAC values");
        memset(alMotorDACValuesServo, 0, sizeof(long) * MAX_OUTPUT_DOF);

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    return HD_CALLBACK_CONTINUE;
}

/*****************************************************************************
 Copies state in thread-safe manner.
*****************************************************************************/
HDCallbackCode HDCALLBACK UpdateMotorDACValuesCallback(void *pUserData)
{
    memcpy(alMotorDACValuesServo,
           alMotorDACValuesApp,
           sizeof(long) * MAX_OUTPUT_DOF);

    return HD_CALLBACK_DONE;
}

/*****************************************************************************
 Selects motor.
*****************************************************************************/
void MotorSelection(int *pMotorIndex)
{    
    do    
    {
        printf("\nEnter the motor index [0 - %d]: ", gNumMotors - 1);
        *pMotorIndex = getch() - '0';
    }
    while (*pMotorIndex < 0 || *pMotorIndex > gNumMotors - 1);

    printf("\nMotor index selected: %d\n", *pMotorIndex);
}

/*****************************************************************************
 Schedules the UpdateMotorDACValuesCallback.
*****************************************************************************/
void SetDACValue(int nMotorIndex)
{
    long nValue;    

    printf("\nSet the DAC value as a number BETWEEN -32768 and 32767 : [-32767 to 32766 ]\n");

    do
    {
	printf("Enter value and press return: ");
#if defined(linux)
	    restore_term();
#endif
	scanf("%d", &nValue);
    } 
    while (nValue < -32768 || nValue > 32767);

    alMotorDACValuesApp[nMotorIndex] = nValue;

    PrintDACValues(alMotorDACValuesApp);

    hdScheduleSynchronous(UpdateMotorDACValuesCallback,
        0, HD_DEFAULT_SCHEDULER_PRIORITY);
}

/* Synchronization structure. */
typedef struct
{
    HDlong encoder_values[MAX_INPUT_DOF];
    HDlong motor_dac_values[MAX_OUTPUT_DOF];   
    hduVector3Dd position;
} DeviceStateStruct;

/*****************************************************************************
 Callback that retrieves state.
*****************************************************************************/
HDCallbackCode HDCALLBACK GetDeviceStateCallback(void *pUserData)
{
    DeviceStateStruct *pState = (DeviceStateStruct *) pUserData;

    hdGetLongv(HD_CURRENT_ENCODER_VALUES, pState->encoder_values);
    hdGetLongv(HD_CURRENT_MOTOR_DAC_VALUES, pState->motor_dac_values);
    hdGetDoublev(HD_CURRENT_POSITION, pState->position);

    return HD_CALLBACK_DONE;
}

/*****************************************************************************
 Callback that retrieves state.
*****************************************************************************/
void PrintDeviceState(HDboolean bContinuous)
{
    int i;
    DeviceStateStruct state;

    memset(&state, 0, sizeof(DeviceStateStruct));

    do
    {
        hdScheduleSynchronous(GetDeviceStateCallback, &state,
            HD_DEFAULT_SCHEDULER_PRIORITY);

        printf("\n");

        printf("Motor DAC Values:");
        for (i = 0; i < gNumMotors; i++)
        {
            printf(" %d", state.motor_dac_values[i]);
        }
        printf("\n");

        printf("Encoder Values:");
        for (i = 0; i < gNumEncoders; i++)
        {
            printf(" %d", state.encoder_values[i]);
        }
        printf("\n");

        printf("Position:");
        for (i = 0; i < 3; i++)
        {
            printf(" %f", state.position[i]);
        }
        printf("\n");

        if (bContinuous)
        {
#if defined(WIN32)
            Sleep(500);
#elif defined(linux)
		struct timespec timeOut;
		timeOut.tv_sec = 0;
		timeOut.tv_nsec = 5*100000000;
		nanosleep(&timeOut, NULL);
#endif
        }

    } while (!_kbhit() && bContinuous);
}

/*******************************************************************************
 Main function.
*******************************************************************************/
int main(int argc, char* argv[])
{  
    HDErrorInfo error;
    HDstring model;
    HDboolean bDone = FALSE;

    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();

        return -1;
    }

    model = hdGetString(HD_DEVICE_MODEL_TYPE);
    printf("Initialized: %s\n", model);

    hdGetIntegerv(HD_OUTPUT_DOF, &gNumMotors);
    hdGetIntegerv(HD_INPUT_DOF, &gNumEncoders);

    memset(alMotorDACValuesApp, 0, sizeof(long) * MAX_OUTPUT_DOF);
    memset(alMotorDACValuesServo, 0, sizeof(long) * MAX_OUTPUT_DOF);

    #if defined(linux)
	nocbreak();
    #endif

    /* Schedule the haptic callback function for continuously monitoring the
       button state and rendering the anchored spring force */
    gCallbackHandle = hdScheduleAsynchronous(
        ServoSchedulerCallback, 0, HD_MAX_SCHEDULER_PRIORITY);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to schedule servoloop callback");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();

        hdDisableDevice(hHD);
        return -1;
    }

    hdEnable(HD_FORCE_OUTPUT);

    /* Start the haptic rendering loop */
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start servoloop");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();

        hdDisableDevice(hHD);
        return -1;        
    }

    PrintHelp();

    /* Start the main application loop */
    mainLoop();

    hdStopScheduler();
    hdUnschedule(gCallbackHandle);
    hdDisableDevice(hHD);

    return 0;
}

/******************************************************************************
 The main loop of execution.  Detects and interprets keypresses.  Monitors and 
 initiates error recovery if necessary.
******************************************************************************/
void mainLoop()
{
    int keypress;
    int nMotorIndex = 0;

    while (TRUE)
    {
        if (_kbhit())
        {
            keypress = getch();
            keypress = toupper(keypress);
            
            switch (keypress)
            {
                case 'M': MotorSelection(&nMotorIndex); break;
                case 'V': SetDACValue(nMotorIndex); break;
                case 'P': PrintDeviceState(FALSE); break;
                case 'C': PrintDeviceState(TRUE); break;
                case 'H': PrintHelp(); break;
                case 'Q': return;
                default: PrintHelp(); break;
            }
        }

        /* Check if the scheduled callback has stopped running */
        if (!hdWaitForCompletion(gCallbackHandle, HD_WAIT_CHECK_STATUS))
        {
            fprintf(stderr, "\nThe main scheduler callback has exited\n");
            fprintf(stderr, "\nPress any key to quit.\n");
            getch();
            return;
        }
    }
}

/*****************************************************************************/
