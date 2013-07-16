/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    hdScheduler.h
    
Description: 

    Thread management utilities for cross thread synchronization

******************************************************************************/

#ifndef HD_SCHEDULER_H_DEFINE
#define HD_SCHEDULER_H_DEFINE

#include <HD/hdExport.h>
#include <HD/hdCompilerConfig.h>
#include <HD/hdDefines.h>

#ifdef __cplusplus
extern "C" 
{
#endif

/* Return code for callbacks, determines whether the operation is finished
   and will unscheduled, or whether it will continue running. */
typedef unsigned int HDCallbackCode;
#define HD_CALLBACK_DONE     0
#define HD_CALLBACK_CONTINUE 1

/* Scheduler operation handle. */
typedef unsigned long HDSchedulerHandle;

/* Callback to scheduler operations. */       
typedef HDCallbackCode (HDCALLBACK *HDSchedulerCallback)(void *pUserData); 

/* WaitForCompletion codes. 
 CHECK_FOR_STATUS returns whether the scheduler operation for
  the given handle is still scheduled. 
 INFINITE waits for the scheduler operation to complete. */
typedef unsigned int HDWaitCode;
#define HD_WAIT_CHECK_STATUS 0
#define HD_WAIT_INFINITE     1

/* Scheduler functions.  Start, stop, or set rate of the scheduler. */
HDAPI void HDAPIENTRY hdStartScheduler();
HDAPI void HDAPIENTRY hdStopScheduler();
HDAPI void HDAPIENTRY hdSetSchedulerRate(HDulong nRate);

/* Schedule a function to run in the scheduler thread and wait for completion. 
   pCallback is the function to be run.
   pUserData is user data to be passed into pCallback.
   nPriority is the scheduling priority, which determines what order the 
   callbacks are run when multiple callbacks are scheduled (higher priority means 
    run first). */
HDAPI void HDAPIENTRY hdScheduleSynchronous(HDSchedulerCallback pCallback, 
                                            void *pUserData, 
                                            HDushort nPriority);
    
/* Schedule a function to run in the scheduler, do not wait for its completion.  
   Returned handle is used to unschedule or block waiting for completion. */
HDAPI HDSchedulerHandle HDAPIENTRY hdScheduleAsynchronous(
                                            HDSchedulerCallback pCallback, 
                                            void *pUserData, 
                                            HDushort nPriority);

/* Unschedule the operation identified by the handle. */
HDAPI void HDAPIENTRY hdUnschedule(HDSchedulerHandle hHandle);

/* Wait until the operation associated with the handle is completed. 
   If HD_WAIT_CHECK_STATUS, returns true if the operation associated
   with the handle is still scheduled */
HDAPI HDboolean HDAPIENTRY hdWaitForCompletion(HDSchedulerHandle hHandle,
                                               HDWaitCode param);

/* Get the time in seconds since the start of the frame. */
HDAPI HDdouble HDAPIENTRY hdGetSchedulerTimeStamp();

#ifdef __cplusplus
}
#endif

#endif /* HD_SCHEDULER_H_DEFINE */

/******************************************************************************/


