/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduRecord.h 

Description: 

  Records device data from the scheduler for a specified number of ticks, and 
  with option to add additional user data.
******************************************************************************/

#ifndef hdRecord_H_
#define hdRecord_H_

#include <HD/hdDefines.h>
#include <stdio.h>

#ifdef __cplusplus 
extern "C" {
#endif  

/******************************************************************************
 For record/playback operations, appends the returned string to the data 
 for each frame.  User is responsible for allocating and deallocating 
 the string.
******************************************************************************/
typedef char *HDURecordCallback (void *pUserData);
    
/******************************************************************************
 Start a recording session.  Records for nData ticks, writes results to file.
 pCallback is an optional user callback called each tick to append any
 additional information.  
 Returns false if recording could not be started.
******************************************************************************/
HDboolean hduStartRecord(
    FILE *file,
    HDURecordCallback pCallback, 
    void *pCallbackData,
    unsigned int nData);

#ifdef __cplusplus
}
#endif

#endif  /* hdRecord_H_ */

/*****************************************************************************/
