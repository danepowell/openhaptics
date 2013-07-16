/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module:

  main.cpp

Description:
  a) show SDK usage with GLUT
  b) a Coulomb force field: one static elec charge, the other elec charge's
         position is changed as haptic cursor
  c) show dual Phantom usage

*******************************************************************************/

#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <cassert>

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

using namespace std;

#include <cstring>

#if defined(WIN32)
#include <conio.h>
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#elif defined(linux)
# include <GL/glut.h>
# include <curses.h>
#endif

#include <cmath>

#include "helper.h"

/* The names of the two haptic devices. */
#define DEVICE_NAME_1  "PHANToM 1"
#define DEVICE_NAME_2  "PHANToM 2"

static double sphereRadius = 12.0;

/* Charge (positive/negative) */
int charge = 1;

HHD phantomId_1, phantomId_2; // Dual Phantom devices.
HDSchedulerHandle gSchedulerCallback = HD_INVALID_HANDLE;

/* Glut callback functions used by helper.cpp */
void displayFuntion(void);
void handleIdle(void );

hduVector3Dd forceField(hduVector3Dd pos);

HDErrorInfo lastError;
/* Haptic device record. */
struct DeviceDisplayStates
{
    HHD m_hHD;
    hduVector3Dd position;
    hduVector3Dd force;
};


/******************************************************************************
 Makes a device specified in the pUserData current.
 Queries haptic device state: position, force, etc. 
******************************************************************************/
HDCallbackCode HDCALLBACK DeviceStateCallback(void *pUserData)
{
    DeviceDisplayStates *pDisplayState = static_cast<DeviceDisplayStates *>(pUserData);

    hdMakeCurrentDevice(pDisplayState->m_hHD);      
    hdGetDoublev(HD_CURRENT_POSITION, pDisplayState->position);
    hdGetDoublev(HD_CURRENT_FORCE, pDisplayState->force);

    return HD_CALLBACK_DONE;
}

/******************************************************************************
 Graphics main loop function.
******************************************************************************/
void displayFunction( void )
{
    // Setup model transformations.
    glMatrixMode( GL_MODELVIEW ); 
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();

    glEnable( GL_COLOR_MATERIAL );

    drawAxes(sphereRadius*3.0);
    setupGraphicsState();

    GLUquadricObj* pQuadObj = gluNewQuadric();

    // First sphere charge.  
    // Get the current position of end effector 1.
    DeviceDisplayStates state_1;
    state_1.m_hHD = phantomId_1;

    hdScheduleSynchronous(DeviceStateCallback, &state_1,
                          HD_MIN_SCHEDULER_PRIORITY);

    // Display one sphere to represent the haptic cursor and the 
    // dynamic charge.
    static const float firstSphereColor[4] = {.2, .8, .8, .8};
    drawSphere(pQuadObj, state_1.position, firstSphereColor, sphereRadius);

    // Second sphere elec charge.
    // Get the current position of end effector 2.
    DeviceDisplayStates state_2;
    state_2.m_hHD = phantomId_2;
    hdScheduleSynchronous(DeviceStateCallback, &state_2,
                          HD_MIN_SCHEDULER_PRIORITY);

    // Display one sphere to represent the haptic cursor and the dynamic 
    // charge.
    static const float secondSphereColor[4] = {.8, .2, .2, .8};
    drawSphere(pQuadObj, state_2.position, secondSphereColor, sphereRadius);

    hduVector3Dd forceVector = 400.0 * forceField(state_2.position-state_1.position);

    glDisable( GL_COLOR_MATERIAL );

    drawForceVector(pQuadObj, state_2.position, forceVector, sphereRadius*.1);

    gluDeleteQuadric(pQuadObj);
    glEnable(GL_LIGHTING);
  
    glPopMatrix();
    glutSwapBuffers();              
}

/*******************************************************************************
 Called periodically by the GLUT framework.
*******************************************************************************/
void handleIdle()
{
    glutPostRedisplay();

    if (!hdWaitForCompletion(gSchedulerCallback, HD_WAIT_CHECK_STATUS))
    {
        printf("The main scheduler callback has exited\n");
        printf("Press any key to quit.\n");
        getchar();
        exit(-1);
    }
}

/******************************************************************************
 Popup menu handler
******************************************************************************/
void handleMenu(int ID)
{
    switch(ID) 
    {
        case 0:
            exit(0);
        case 1:
            charge *= -1;
            break;
    }
}

/*******************************************************************************
 Given the position is space, calculates the (modified) coulomb force.
*******************************************************************************/
hduVector3Dd forceField(hduVector3Dd pos)
{
    double dist = pos.magnitude();
    
    hduVector3Dd forceVec(0,0,0);
    
    // if two charges overlap...
    if(dist < sphereRadius*2.0) 
    {
        // Attract the charge to the center of the sphere.
        forceVec =  -0.1*pos;
    }
    else
    {
        hduVector3Dd unitPos = normalize(pos);
        forceVec = -1200.0*unitPos/(dist*dist);
    }
    forceVec *= charge;

    return forceVec;
}

/******************************************************************************
 Main callback.  Retrieves position from both devices, calculates forces,
 and sets forces for both devices.
******************************************************************************/
HDCallbackCode HDCALLBACK DualCoulombCallback(void *data)
{
    hduVector3Dd pos_1, pos_2;

    hdBeginFrame(phantomId_1);
    hdGetDoublev(HD_CURRENT_POSITION,pos_1);

    hdBeginFrame(phantomId_2);
    hdGetDoublev(HD_CURRENT_POSITION,pos_2);

    hduVector3Dd pos_diff = pos_1 - pos_2;

    hduVector3Dd forceVec = forceField(pos_diff);

    hdMakeCurrentDevice(phantomId_1);
    hdSetDoublev(HD_CURRENT_FORCE, forceVec);

    // Flush forces on the first device.
    hdEndFrame(phantomId_1);

    // The second haptic device feels the opposing force.
    forceVec = -1.0 * forceVec;

    hdMakeCurrentDevice(phantomId_2);
    hdSetDoublev(HD_CURRENT_FORCE, forceVec);
    
    // Flush forces on the second device.
    hdEndFrame(phantomId_2);

    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Error during scheduler callback");

        if (hduIsSchedulerError(&error))
        {
            return HD_CALLBACK_DONE;
        }
    }

    return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
 Initializes the coulomb force field callback.
******************************************************************************/
void DualCoulombForceField()
{
    std::cout << "haptics callback" << std::endl;
    gSchedulerCallback = hdScheduleAsynchronous(
        DualCoulombCallback, 0, HD_DEFAULT_SCHEDULER_PRIORITY);

    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        exit(-1);
    }

    /* Enter GLUT main loop. */
    glutMainLoop();                 
}

/******************************************************************************
 This handler gets called when the process is exiting.  Ensures that HDAPI is
 properly shutdown.
******************************************************************************/
void exitHandler()
{
   
   if (!lastError.errorCode)
    {
        hdStopScheduler();
        hdUnschedule(gSchedulerCallback);
    }

    if (phantomId_1 != HD_INVALID_HANDLE)
    {
        hdDisableDevice(phantomId_1);
        phantomId_1 = HD_INVALID_HANDLE;
    }

    if (phantomId_2 != HD_INVALID_HANDLE)
    {
        hdDisableDevice(phantomId_2);
        phantomId_2 = HD_INVALID_HANDLE;
    }
}
/******************************************************************************
 Main entry point.
******************************************************************************/
int main(int argc, char* argv[])
{
    HDErrorInfo error;

    printf("Starting application\n");
                                                                                   
    atexit(exitHandler);

    // Initialize both devices.  These needs to be called before any actions 
    // on the devices.

    // First device.
    phantomId_1 = hdInitDevice(DEVICE_NAME_1);
    if (HD_DEVICE_ERROR(lastError = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize first haptic device");
        fprintf(stderr, "Make sure the configuration \"%s\" exists\n", DEVICE_NAME_1);
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        exit(-1);
    }

    printf("1. Found device %s\n",hdGetString(HD_DEVICE_MODEL_TYPE));
    hdEnable(HD_FORCE_OUTPUT);
    hdEnable(HD_FORCE_RAMPING);

    // Second device.
    phantomId_2 = hdInitDevice(DEVICE_NAME_2);
    if (HD_DEVICE_ERROR(lastError = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize second haptic device");
        fprintf(stderr, "Make sure the configuration \"%s\" exists\n", DEVICE_NAME_2);
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        exit(-1);
    }

    printf("2. Found device %s\n",hdGetString(HD_DEVICE_MODEL_TYPE));
    hdEnable(HD_FORCE_OUTPUT);
    hdEnable(HD_FORCE_RAMPING);

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        exit(-1);
    }

    // Initialize GLUT.
    initGlut( argc, argv );         

    // Get the workspace dimensions.
    HDdouble maxWorkspace[6];
    hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);

    // Low/left/back point of device workspace.
    hduVector3Dd LLB(maxWorkspace[0], maxWorkspace[1], maxWorkspace[2]);
    // Top/right/front point of device workspace.
    hduVector3Dd TRF(maxWorkspace[3], maxWorkspace[4], maxWorkspace[5]);
    initGraphics(LLB, TRF);

    hdMakeCurrentDevice(phantomId_1);

    // Application loop.
    DualCoulombForceField();

    printf("Done\n");
    return 0;
}

            
