/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ServoLoopRate.cpp
        
Description: 

  This application serves as a unit test for collecting statistics on the
  update intervals of the device.

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <iostream>
#include <fstream>
#include <stdio.h>

#if defined(WIN32)
# include <windows.h>
# include <conio.h>
#else
# include <unistd.h>
# define Sleep(x) usleep((x) * 1000)
# include "conio.h"
#endif

#include <HD/hd.h>
#include <HDU/hduError.h>

#include "StatsSampler.h"

#define TARGET_SERVOLOOP_RATE   1000

StatsSamplerd gUpdateRateServo;
StatsSamplerd gUpdateRateApp;

/******************************************************************************
 Prints the update rate statistics to the specified output stream
******************************************************************************/
void PrintUpdateRateStats(std::ostream &os)
{
    os << "Rate: " << gUpdateRateApp << std::endl;
}

/******************************************************************************
 The main servo loop scheduler callback.  Samples the haptic frame rate.
******************************************************************************/
HDCallbackCode HDCALLBACK ServoSchedulerCallback(void *pUserData)
{
    hdBeginFrame(hdGetCurrentDevice());    

    HDdouble rate;
    hdGetDoublev(HD_INSTANTANEOUS_UPDATE_RATE, &rate);

    hdEndFrame(hdGetCurrentDevice());

    gUpdateRateServo.sample(rate);

    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << std::endl;
        std::cerr << error << std::endl;

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
 Use this callback synchronously.  Makes a thread-safe copy of the servo
 loop rate statistics.
******************************************************************************/
HDCallbackCode HDCALLBACK CopyUpdateRateStats(void *pUserData)
{
    gUpdateRateApp = gUpdateRateServo;

    gUpdateRateServo.clear();

    return HD_CALLBACK_DONE;    
}

/******************************************************************************
 Collects statistics about the update rate of the haptic device.
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

    HDstring model = hdGetString(HD_DEVICE_MODEL_TYPE);
    std::cout << "Initialized: " << model << std::endl;

    HDSchedulerHandle hServoCallback = hdScheduleAsynchronous(
        ServoSchedulerCallback, 0, HD_MAX_SCHEDULER_PRIORITY);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << error << std::endl;
        std::cerr << "Failed to schedule servoloop callback" << std::endl;
        hdDisableDevice(hHD);
        return -1;
    }

    hdSetSchedulerRate(TARGET_SERVOLOOP_RATE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << error << std::endl;
        std::cerr << "Failed to set servoloop rate" << std::endl;
        hdDisableDevice(hHD);
        return -1;        
    }

    hdDisable(HD_FORCE_OUTPUT);

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << error << std::endl;
        std::cerr << "Failed to start servoloop" << std::endl;
        hdDisableDevice(hHD);
        return -1;        
    }

    std::cout << "Printing servoloop rate stats. All numbers are in Hz units" << std::endl;
    std::cout << std::endl;
        
    char fileName[256];
    sprintf(fileName, "%s Rate Stats.txt", model);
        std::ofstream fout(fileName, std::ios::out | std::ios::app); 

    for (int nRuns = 0; nRuns < 10 && !_kbhit(); nRuns++)
    {
        Sleep(1000);

        hdScheduleSynchronous(CopyUpdateRateStats, 0, HD_MIN_SCHEDULER_PRIORITY);

        // Prints some stats about the rate as well as log it to file.
        PrintUpdateRateStats(std::cout);
        PrintUpdateRateStats(fout);

        if (!hdWaitForCompletion(hServoCallback, HD_WAIT_CHECK_STATUS))
        {
            std::cerr << "Error occurred during main loop" << std::endl;
            std::cerr << "Press any key to quit." << std::endl;
            getch();
            break;
        }
    }

    hdStopScheduler();
    hdUnschedule(hServoCallback);
    hdDisableDevice(hHD);

    return 0;
}

/*****************************************************************************/

