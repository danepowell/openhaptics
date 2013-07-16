/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduRecord.cpp

Description: 

  Records device data from the scheduler for a specified number of ticks, and 
  with option to add additional user data.

******************************************************************************/

#include "hduAfx.h"

#include <iostream>

#include <HDU/hduRecord.h>
#include <HDU/hduVector.h>

#include <HD/hdCompilerConfig.h>
#include <HD/hdScheduler.h>
#include <HD/hdDevice.h>

#if defined WIN32
# include <process.h>
  typedef void thread_rv;
#elif defined(linux) || defined(__APPLE__)
# include <pthread.h>
  typedef void* thread_rv;
#endif

/* Scheduler callback for getting data for each iteration. */
HDCallbackCode HDCALLBACK recordLoopCallback(void *_hduRecorder);

/* Thread callback for writing out data to file. */
thread_rv recordWriteCallback( void *data );

/******************************************************************************
 Data packet for each iteration. 
******************************************************************************/
struct hduRecorderPacket
{
    hduVector3Dd m_force;
    hduVector3Dd m_position;
    hduVector3Dd m_velocity;
    char *m_userData;
};

/******************************************************************************
 Main class that handles recording. 
******************************************************************************/
class hduRecorder
{
public:

    hduRecorder(FILE *file,
                HDURecordCallback &pUserCallback,
                void *pUserCallbackData,
                int numData);

    virtual ~hduRecorder();

    HDboolean start();
    HDboolean addData();
    void writeDataToFile();

private:

    HDURecordCallback *m_pUserCallback;
    void *m_pUserCallbackData;
    FILE *m_file;
    int m_numData;
    int m_index;
    hduRecorderPacket *m_data;
};

/******************************************************************************
 Constructor
******************************************************************************/
hduRecorder::hduRecorder(FILE *file,
                         HDURecordCallback &pUserCallback,
                         void *pUserCallbackData,
                         int numData) :
    m_file(file),
    m_pUserCallback(pUserCallback),
    m_pUserCallbackData(pUserCallbackData),
    m_numData(numData),
    m_index(0),
    m_data(0)
{
    m_data = new hduRecorderPacket[numData];
}

/******************************************************************************
 Destructor
******************************************************************************/
hduRecorder::~hduRecorder()
{
    for (int i=0; i < m_numData; i++)
    {
        delete m_data[i].m_userData;
    }
    delete[] m_data;
}

/******************************************************************************
 Start the recording.  Schedule a callback that records device data for 
 every scheduler tick. 
******************************************************************************/
HDboolean hduRecorder::start()
{
    HDSchedulerHandle h = hdScheduleAsynchronous(recordLoopCallback,
                                                 this,
                                                 HD_MIN_SCHEDULER_PRIORITY);
    return (h == HD_INVALID_HANDLE ? false : true);
}

/******************************************************************************
 Worker thread callback to write the data to file.
******************************************************************************/
thread_rv recordWriteCallback( void *data )
{
    hduRecorder *pRecord = (hduRecorder *)data;
    pRecord->writeDataToFile();
    delete pRecord;
}

/******************************************************************************
 Write all the data out at once when the recording is done.
******************************************************************************/
void hduRecorder::writeDataToFile()
{
    for (int i=0; i < m_numData; i++)
    {
        hduRecorderPacket &data = m_data[i];
        fprintf(m_file,"%d\t",i);
        fprintf(m_file,"%f %f %f\t\t",data.m_force[0],
                data.m_force[1],
                data.m_force[2]);
        fprintf(m_file,"%f %f %f\t\t",data.m_position[0],
                data.m_position[1],
                data.m_position[2]);
        fprintf(m_file,"%f %f %f\t\t",data.m_velocity[0],
                data.m_velocity[1],
                data.m_velocity[2]);
        fprintf(m_file,"%s\n",data.m_userData);
    }
}

/******************************************************************************
 Scheduler callback; at each scheduler tick, add one tick's worth of data.
******************************************************************************/
HDCallbackCode HDCALLBACK recordLoopCallback(void *_hduRecorder)
{
    hduRecorder *pRecord = (hduRecorder *)_hduRecorder;
    HDboolean bDone = pRecord->addData();

    /* If done, then create another thread to write out the data. */
    if (bDone)
    {
#if defined(WIN32)
        _beginthread(recordWriteCallback, 0, pRecord);
#elif defined(linux) || defined(__APPLE__)
        pthread_create(NULL, NULL, recordWriteCallback, pRecord);
#endif
        return HD_CALLBACK_DONE;
    }
    return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
 Adds data for one tick.  Return true if done
******************************************************************************/
HDboolean hduRecorder::addData()
{
    /* Get the data and fill one packet. */
    static hduRecorderPacket data;
    hdGetDoublev(HD_CURRENT_FORCE,data.m_force);
    hdGetDoublev(HD_CURRENT_POSITION,data.m_position);
    hdGetDoublev(HD_CURRENT_VELOCITY,data.m_velocity);
    data.m_userData = 0;
    if (m_pUserCallback)
    {
        char *userData = (*m_pUserCallback)(m_pUserCallbackData);
        data.m_userData = userData;
    }

    /* Either add the data to storage, or stop getting data if the recorder 
       has received the amount specified. */
    if (m_index < m_numData)
    {
        m_data[m_index] = data;
        m_index++;
        return false;
    }
    else     
    {
        return true;
    }
}

/******************************************************************************
 Create a recorder, start recording.  
******************************************************************************/
HDboolean hdStartRecord(
    FILE *file,
    HDURecordCallback pCallback,
    void *pCallbackData,
    unsigned int nData)
{
    if (!file)
    {
        return false;
    }
    hduRecorder *pRecord = new hduRecorder(file,
                                           *pCallback,
                                           pCallbackData,
                                           nData);
    if (pRecord->start())
    {
        return true;
    }
    else
    {
        delete(pRecord);
        return false;
    }
}

/*****************************************************************************/
