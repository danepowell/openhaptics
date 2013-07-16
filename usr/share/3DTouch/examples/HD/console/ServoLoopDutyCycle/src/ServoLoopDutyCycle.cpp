/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  ServoLoopDutyCycle.c

Description: 

  This application serves as a unit test for timing the minimum required
  interface calls for a haptic device control loop, which involves the 3 main
  timing blocks:
    1) The overhead for the scheduler (i.e. the timing mechanism)
    2) The time to refresh the state of the device
    3) The time to command a force to the device

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

StatsSamplerd gSchedulerStatsServo;
StatsSamplerd gInputStatsServo;
StatsSamplerd gOutputStatsServo;

StatsSamplerd gSchedulerStatsApp;
StatsSamplerd gInputStatsApp;
StatsSamplerd gOutputStatsApp;

/******************************************************************************
 Prints the duty cycles stats to the output stream provided.  
 The StatsSamplerd class has an ostream operator that handles formatting 
 the relevant data
******************************************************************************/
void PrintDutyCycleStats(std::ostream &os)
{
    os.precision(3);

    os << std::endl;

    os << "Scheduler: " << gSchedulerStatsApp << std::endl;
    os << "Input: " << gInputStatsApp << std::endl;
    os << "Output: " << gOutputStatsApp << std::endl;

    double totalMean = gSchedulerStatsApp.u() + 
                       gInputStatsApp.u() +
                       gOutputStatsApp.u();
    double totalMin = gSchedulerStatsApp.getMin() +
                      gInputStatsApp.getMin() +
                      gOutputStatsApp.getMin();
    double totalMax = gSchedulerStatsApp.getMax() +
                      gInputStatsApp.getMax() +
                      gOutputStatsApp.getMax();

    os << "Summary: " 
       << "total mean " << totalMean << " "
       << "total min " << totalMin << " "
       << "total max " << totalMax << std::endl;
}

/******************************************************************************
 Captures stats on the duty cycle of the servo loop.
******************************************************************************/
HDCallbackCode HDCALLBACK ServoSchedulerCallback(void *pUserData)
{
    // This is the time from the start of the loop until this scheduler
    // callback was called.
    HDdouble startTime = hdGetSchedulerTimeStamp();

    hdBeginFrame(hdGetCurrentDevice());    

    // Capture the time it took to perform an update on the device.
    HDdouble updateTime = hdGetSchedulerTimeStamp();

    float force[3] = { 0, 0, 0 };
    hdSetFloatv(HD_CURRENT_FORCE, force);

    hdEndFrame(hdGetCurrentDevice());

    // Capture the time it took to set a force and flush it to the device.
    HDdouble endTime = hdGetSchedulerTimeStamp();
    
    // Sample values and convert to ms.
    gSchedulerStatsServo.sample(1000 * startTime);
    gInputStatsServo.sample(1000 * (updateTime - startTime));
    gOutputStatsServo.sample(1000 * (endTime - updateTime));

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
 Call this sychronously from the main thread.  Obtains a thread-safe copy of 
 the duty cycle stats.
******************************************************************************/
HDCallbackCode HDCALLBACK CopyDutyCycleStats(void *pUserData)
{
    gSchedulerStatsApp = gSchedulerStatsServo;
    gInputStatsApp = gInputStatsServo;
    gOutputStatsApp = gOutputStatsServo;

    gSchedulerStatsServo.clear();
    gInputStatsServo.clear();
    gOutputStatsServo.clear();

    return HD_CALLBACK_DONE;    
}

/******************************************************************************
 Runs ths haptic device servo loop and captures stats on its idle duty cycle.
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

    hdEnable(HD_FORCE_OUTPUT);
    hdDisable(HD_FORCE_RAMPING);

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << error << std::endl;
        std::cerr << "Failed to start servoloop" << std::endl;        
        hdDisableDevice(hHD);
        return -1;        
    }

    std::cout << "Printing servoloop duty cycle stats. All times are in ms units." 
              << std::endl;
        
    char fileName[256];
    sprintf(fileName, "%s Duty Cycle Stats.txt", model);
        std::ofstream fout(fileName, std::ios::out | std::ios::app); 

    for (int nRuns = 0; nRuns < 10 && !_kbhit(); nRuns++)
    {
        /* Print the average update DutyCycle every second. */
        Sleep(1000);

        hdScheduleSynchronous(CopyDutyCycleStats, 0, HD_MIN_SCHEDULER_PRIORITY);

        PrintDutyCycleStats(std::cout);
        PrintDutyCycleStats(fout);

        if (!hdWaitForCompletion(hServoCallback, HD_WAIT_CHECK_STATUS))
        {
            std::cerr << "Error during main loop of application" << std::endl;
            std::cout << "Press any key to quit." << std::endl;
            getch();
            break;
        }
    }

    hdStopScheduler();
    hdUnschedule(hServoCallback);
    hdDisableDevice(hHD);

    return 0;
}

/******************************************************************************/
