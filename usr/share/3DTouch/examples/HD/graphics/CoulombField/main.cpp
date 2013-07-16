/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  main.cpp

Description:

  The main file that performs all haptics-relevant operation. Within a 
  asynchronous callback the graphics thread reads the position and sets
  the force. Within a synchronous callback the graphics thread gets the
  position and constructs graphics elements (e.g. force vector).

*******************************************************************************/

#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <cassert>

#include <HD/hd.h>

#include "helper.h"

#include <HDU/hduError.h>
#include <HDU/hduVector.h>

static double sphereRadius = 12.0;

/* Charge (positive/negative) */
int charge = 1;

static HHD ghHD = HD_INVALID_HANDLE;
static HDSchedulerHandle gSchedulerCallback = HD_INVALID_HANDLE;

/* Glut callback functions used by helper.cpp */
void displayFunction(void);
void handleIdle(void);

hduVector3Dd forceField(hduVector3Dd pos);

/* Haptic device record. */
struct DeviceDisplayState
{
    HHD m_hHD;
    hduVector3Dd position;
    hduVector3Dd force;
};

/*******************************************************************************
 Client callback used by the graphics main loop function.
 Use this callback synchronously.
 Gets data, in a thread safe manner, that is constantly being modified by the 
 haptics thread. 
*******************************************************************************/
HDCallbackCode HDCALLBACK DeviceStateCallback(void *pUserData)
{
    DeviceDisplayState *pDisplayState = 
        static_cast<DeviceDisplayState *>(pUserData);

    hdGetDoublev(HD_CURRENT_POSITION, pDisplayState->position);
    hdGetDoublev(HD_CURRENT_FORCE, pDisplayState->force);

    // execute this only once.
    return HD_CALLBACK_DONE;
}


/*******************************************************************************
 Graphics main loop function.
*******************************************************************************/
void displayFunction(void)
{
    // Setup model transformations.
    glMatrixMode(GL_MODELVIEW); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    setupGraphicsState();
    drawAxes(sphereRadius*3.0);

    // Draw the fixed sphere.
    static const hduVector3Dd fixedSpherePosition(0, 0, 0);
    static const float fixedSphereColor[4] = {.2, .8, .8, .8};
    GLUquadricObj* pQuadObj = gluNewQuadric();
    drawSphere(pQuadObj, fixedSpherePosition, fixedSphereColor, sphereRadius);

    // Get the current position of end effector.
    DeviceDisplayState state;
    hdScheduleSynchronous(DeviceStateCallback, &state,
                          HD_MIN_SCHEDULER_PRIORITY);

    // Draw a sphere to represent the haptic cursor and the dynamic 
    // charge.
    static const float dynamicSphereColor[4] = { .8, .2, .2, .8 };
    drawSphere(pQuadObj, 
               state.position,
               dynamicSphereColor,
               sphereRadius);    

    // Create the force vector.
    hduVector3Dd forceVector = 400.0 * forceField(state.position);
    
    drawForceVector(pQuadObj,
                    state.position,
                    forceVector,
                    sphereRadius*.1);

    gluDeleteQuadric(pQuadObj);
  
    glPopMatrix();
    glutSwapBuffers();                      
}
                                
/*******************************************************************************
 Called periodically by the GLUT framework.
*******************************************************************************/
void handleIdle(void)
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
            break;
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

/*******************************************************************************
 Main callback that calculates and sets the force.
*******************************************************************************/
HDCallbackCode HDCALLBACK CoulombCallback(void *data)
{
    HHD hHD = hdGetCurrentDevice();

    hdBeginFrame(hHD);

    hduVector3Dd pos;
    hdGetDoublev(HD_CURRENT_POSITION,pos);
    hduVector3Dd forceVec;
    forceVec = forceField(pos);
    hdSetDoublev(HD_CURRENT_FORCE, forceVec);
        
    hdEndFrame(hHD);

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

/*******************************************************************************
 Schedules the coulomb force callback.
*******************************************************************************/
void CoulombForceField()
{
    std::cout << "haptics callback" << std::endl;
    gSchedulerCallback = hdScheduleAsynchronous(
        CoulombCallback, 0, HD_DEFAULT_SCHEDULER_PRIORITY);

    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        exit(-1);
    }

    std::cout << "graphics callback" << std::endl;

    glutMainLoop(); // Enter GLUT main loop.
}

/******************************************************************************
 This handler gets called when the process is exiting. Ensures that HDAPI is
 properly shutdown
******************************************************************************/
void exitHandler()
{
    hdStopScheduler();
    hdUnschedule(gSchedulerCallback);

    if (ghHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(ghHD);
        ghHD = HD_INVALID_HANDLE;
    }
}

/******************************************************************************
 Main function.
******************************************************************************/
int main(int argc, char* argv[])
{
    HDErrorInfo error;

    printf("Starting application\n");
    
    atexit(exitHandler);

    // Initialize the device.  This needs to be called before any other
    // actions on the device are performed.
    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        exit(-1);
    }

    printf("Found device %s\n",hdGetString(HD_DEVICE_MODEL_TYPE));
    
    hdEnable(HD_FORCE_OUTPUT);
    hdEnable(HD_MAX_FORCE_CLAMPING);

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        exit(-1);
    }
    
    initGlut(argc, argv);

    // Get the workspace dimensions.
    HDdouble maxWorkspace[6];
    hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);

    // Low/left/back point of device workspace.
    hduVector3Dd LLB(maxWorkspace[0], maxWorkspace[1], maxWorkspace[2]);
    // Top/right/front point of device workspace.
    hduVector3Dd TRF(maxWorkspace[3], maxWorkspace[4], maxWorkspace[5]);
    initGraphics(LLB, TRF);

    // Application loop.
    CoulombForceField();

    printf("Done\n");
    return 0;
}

/******************************************************************************/
