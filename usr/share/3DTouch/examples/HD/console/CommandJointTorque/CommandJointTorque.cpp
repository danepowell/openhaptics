/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  CommandJointTorque.cpp

Description: 

  This example demonstrates commanding joint torques to the Phantom device. 
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
#include <time.h>
# include "conio.h"
# include <string.h>
#define FALSE 0
#define TRUE 1
#endif

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

static const hduVector3Dd maxGimbalTorque(188.0,188.0,48.0); //mNm
static const hduVector3Dd nominalBaseTorque(200.0,350.0,200.0); //mNm
static bool TorqueMode = true;

HDCallbackCode HDCALLBACK jointTorqueCallback(void *data);
HDSchedulerHandle hGravityWell = HD_INVALID_HANDLE;

void mainLoop(void);
bool initDemo(void);

void PrintHelp()
{
    static const char help[] = {\
"CommandJointTorque Help\n\
---\n\
T: Command Base & Gimbal Torques \n\
   Torsional Springs at each Joint\n\
   Torsion Spring Constant at Joints = 1000 mN.m/radian\n\
   Torsion Spring Constant at Gimbals = 500 mN.m/radian\n\
   Max Base Torque Commanded by this Demo = {200.0,350.0,200.0}mNm\n\
   Max Gimbal Torque Commanded by this Demo = {188.0,188.0,48.0}mNm\n\
F: Command Base force & Gimbal Torque\n\
   Torsional Springs at Gimbals & a Extension Spring at Position(0,0,0)\n\
   Extension Spring Constant at (0,0,0) = 0.075 N/mm\n\
   Torsion Spring Constant at Gimbals = 500 mN.m/radian\n\
P: Prints device state\n\
C: Continuously prints device state\n\
H: Prints help menu\n\
Q: Quits the program\n\
---"};
    
    printf("\n%s\n", help);
}

/* Synchronization structure. */
typedef struct
{
    HDdouble forceValues[3];
    HDdouble jointTorqueValues[3];   
    HDdouble gimbalTorqueValues[3];   
} DeviceStateStruct;

/*****************************************************************************
 Callback that retrieves state.
*****************************************************************************/
HDCallbackCode HDCALLBACK GetDeviceStateCallback(void *pUserData)
{
    DeviceStateStruct *pState = (DeviceStateStruct *) pUserData;

    hdGetDoublev(HD_CURRENT_FORCE, pState->forceValues);
    hdGetDoublev(HD_CURRENT_JOINT_TORQUE, pState->jointTorqueValues);
    hdGetDoublev(HD_CURRENT_GIMBAL_TORQUE, pState->gimbalTorqueValues);

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

        if (TorqueMode)
        {
        printf("Current Base Torque Values (mNm):");
        for (i = 0; i < 3; i++)
        {
            printf(" %f", state.jointTorqueValues[i]);
        }
        printf("\n");

        printf("Current Gimbal Torque Values (mNm):");
        for (i = 0; i < 3; i++)
        {
            printf(" %f", state.gimbalTorqueValues[i]);
        }
        printf("\n");
        }
        else
        {
        printf("Current Base Force Values (N):");
        for (i = 0; i < 3; i++)
        {
            printf(" %f", state.forceValues[i]);
        }
        printf("\n");

        printf("Current Base Torque Values (mNm):");
        for (i = 0; i < 3; i++)
        {
            printf(" %f", state.jointTorqueValues[i]);
        }
        printf("\n");

        printf("Current Gimbal Torque Values (mNm):");
        for (i = 0; i < 3; i++)
        {
            printf(" %f", state.gimbalTorqueValues[i]);
        }
        printf("\n");
        }

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
 Initializes the device, starts the schedule, creates a schedule callback
 to handle gravity well forces, waits for the user to press a button, exits
 the application.
*******************************************************************************/
int main(int argc, char* argv[])
{    
    HDErrorInfo error;
    /* Initialize the device, must be done before attempting to call any hd 
       functions. Passing in HD_DEFAULT_DEVICE causes the default device to be 
       initialized. */
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError())) 
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }

    printf("Command Joint Torque Demo!\n");
    printf("Found device model: %s.\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));

    if (!initDemo())
    {
        printf("Demo Initialization failed\n");
        printf("Press any key to exit\n");
        getch();
        
    }

    /* Schedule the main callback that will render forces to the device. */
    hGravityWell = hdScheduleAsynchronous(
        jointTorqueCallback, 0, 
        HD_MAX_SCHEDULER_PRIORITY);

    hdEnable(HD_FORCE_OUTPUT);
    hdStartScheduler();

    /* Check for errors and abort if so. */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        return -1;
    }

    PrintHelp();

    /* Start the main application loop */
    mainLoop();

    /* For cleanup, unschedule callback and stop the scheduler. */
    hdStopScheduler();
    hdUnschedule(hGravityWell);

    /* Disable the device. */
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
                case 'F': TorqueMode = false; break;
                case 'T': TorqueMode = true; break;
                case 'P': PrintDeviceState(FALSE); break;
                case 'C': PrintDeviceState(TRUE); break;
                case 'H': PrintHelp(); break;
                case 'Q': return;
                default: PrintHelp(); break;
            }
        }

        /* Check if the scheduled callback has stopped running */
        if (!hdWaitForCompletion(hGravityWell, HD_WAIT_CHECK_STATUS))
        {
            fprintf(stderr, "\nThe main scheduler callback has exited\n");
            fprintf(stderr, "\nPress any key to quit.\n");
            getch();
            return;
        }
    }
}

/*******************************************************************************
 Servo callback.  
 Called every servo loop tick.  Simulates a gravity well, which sucks the device 
 towards its center whenever the device is within a certain range.
*******************************************************************************/
HDCallbackCode HDCALLBACK jointTorqueCallback(void *data)
{
    const HDdouble kStiffness = 0.075; /* N/mm */
    const HDdouble kStylusTorqueConstant = 500; /* torque spring constant (mN.m/radian)*/
    const HDdouble kJointTorqueConstant = 1000; /* torque spring constant (mN.m/radian)*/
 
    const HDdouble kForceInfluence = 50; /* mm */
    const HDdouble kTorqueInfluence = 3.14; /* radians */

    /* This is the position of the gravity well in cartesian
       (i.e. x,y,z) space. */
    static const hduVector3Dd wellPos(0,0,0);
    static const hduVector3Dd stylusVirtualFulcrum(0.0, 0.0, 0.0); // In radians
    static const hduVector3Dd jointVirtualFulcrum(0.0, 0.0, 0.0); // In radians
    
    HDErrorInfo error;
    hduVector3Dd position;

    hduVector3Dd force;
    hduVector3Dd positionTwell;
    hduVector3Dd gimbalAngles;
    hduVector3Dd gimbalTorque;
    hduVector3Dd gimbalAngleOfTwist;
    hduVector3Dd jointAngles;
    hduVector3Dd jointTorque;
    hduVector3Dd jointAngleOfTwist;

    HHD hHD = hdGetCurrentDevice();

    /* Begin haptics frame.  ( In general, all state-related haptics calls
       should be made within a frame. ) */
    hdBeginFrame(hHD);

    /* Get the current position of the device. */
    hdGetDoublev(HD_CURRENT_POSITION, position);
    hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES,gimbalAngles );
    hdGetDoublev(HD_CURRENT_JOINT_ANGLES,jointAngles );

    memset(force, 0, sizeof(hduVector3Dd));
    

    /* >  positionTwell = wellPos-position  < 
       Create a vector from the device position towards the gravity 
       well's center. */
    hduVecSubtract(positionTwell, wellPos, position);
    
    hduVecSubtract(gimbalAngleOfTwist, stylusVirtualFulcrum, gimbalAngles);

    hduVecSubtract(jointAngleOfTwist, jointVirtualFulcrum, jointAngles);

    /* If the device position is within some distance of the gravity well's 
       center, apply a spring force towards gravity well's center.  The force
       calculation differs from a traditional gravitational body in that the
       closer the device is to the center, the less force the well exerts;
       the device behaves as if a spring were connected between itself and
       the well's center. */
    if (hduVecMagnitude(positionTwell) < kForceInfluence)
    {
        /* >  F = k * x  < 
           F: Force in Newtons (N)
           k: Stiffness of the well (N/mm)
           x: Vector from the device endpoint position to the center 
           of the well. */
        hduVecScale(force, positionTwell, kStiffness);
    }

    if (hduVecMagnitude(gimbalAngleOfTwist) < kTorqueInfluence)
    {
        /* >  T = k * r  < We calculate torque by assuming a torsional spring at each joint. 
           T: Torque in Milli Newton . meter (mN.m)
           k: Torque Spring Constant (mN.m / radian)
           r: Angle of twist from fulcrum point (radians)*/
        hduVecScale(gimbalTorque, gimbalAngleOfTwist, kStylusTorqueConstant);
    }
    
    if (hduVecMagnitude(jointAngleOfTwist) < kTorqueInfluence)
    {
        /* >  T = k * r  < We calculate torque by assuming a torsional spring at each joint. 
           T: Torque in Milli Newton . meter (mN.m)
           k: Torque Spring Constant (mN.m / radian)
           r: Angle of twist from fulcrum point (radians)*/

        hduVecScale(jointTorque, jointAngleOfTwist, kJointTorqueConstant);
    }


// Clamp the base torques to the nominal values. 
    for (int i=0; i<3; i++)
    {
        if (jointTorque[i] > nominalBaseTorque[i])
            jointTorque[i] = nominalBaseTorque[i];
        else if (jointTorque[i] < -nominalBaseTorque[i])
            jointTorque[i] = -nominalBaseTorque[i];
    }

// Clamp the gimbal torques to the max continous values. 
    for (int i=0; i<3; i++)
    {
        if (gimbalTorque[i] > maxGimbalTorque[i])
            gimbalTorque[i] = maxGimbalTorque[i];
        else if (gimbalTorque[i] < -maxGimbalTorque[i])
            gimbalTorque[i] = -maxGimbalTorque[i];
    }


    /* Send the forces & torques to the device. */
    /*Switch back between sending forces & torques 
    to the base motors */
    if (!TorqueMode)
        hdSetDoublev(HD_CURRENT_FORCE, force);
    else
        hdSetDoublev(HD_CURRENT_JOINT_TORQUE, jointTorque);

    hdSetDoublev(HD_CURRENT_GIMBAL_TORQUE, gimbalTorque);

//    printf("%f\t%f\t%f\n\n", jointTorque[0], jointTorque[1], jointTorque[2]);
    /* End haptics frame. */
    hdEndFrame(hHD);

    /* Check for errors and abort the callback if a scheduler error
       is detected. */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, 
                      "Error detected while rendering gravity well\n");
        
        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    /* Signify that the callback should continue running, i.e. that
       it will be called again the next scheduler tick. */
    return HD_CALLBACK_CONTINUE;
}

bool initDemo(void)
{
    HDErrorInfo error;
    int calibrationStyle;
    printf("Calibration\n");

    hdGetIntegerv(HD_CALIBRATION_STYLE, &calibrationStyle);
    if (calibrationStyle & HD_CALIBRATION_AUTO || calibrationStyle & HD_CALIBRATION_INKWELL)
    {
        printf("Please prepare for starting the demo by \n");
        printf("placing the device at its reset position.\n\n");
        printf("Press any key to continue...\n");
        getch();
        return 1;
    }
    if (calibrationStyle & HD_CALIBRATION_ENCODER_RESET )
    {
        printf("Please prepare for starting the demo by \n");
        printf("placing the device at its reset position.\n\n");
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
/*****************************************************************************/
