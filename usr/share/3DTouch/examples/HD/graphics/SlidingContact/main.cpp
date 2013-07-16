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

  The main file that performs all haptics-relevant operation.  At a high level,
  a ContactModel object instance is used by both the graphics and haptics
  sides, in a thread safe manner.  In the haptics side, the ContactModel 
  instance is passed as client data to an asynchronous callback that performs 
  the dynamic simulation.  This ContactModel instance is not used directly 
  by the graphics main loop function, as this would not be thread safe. Instead,
  we define an auxiliary datastructure called Synchronizer. This data structure
  basically duplicates the interesting (from the graphics point of view) data,
  and it is passed as client data to a synchronous callback to the haptics side.
  Within the callback, data is duplicated from the ContactModel instance
  to auxiliary fields that are not modified by the haptics loop.

*******************************************************************************/

#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <cmath>

#include <HD/hd.h>
#include <HDU/hduError.h>

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

#include "Constants.h"
#include "ContactModel.h"

/*******************************************************************************
 Globals, declarations etc 
*******************************************************************************/

/* Center of fixed sphere, initialized to be the center of
   haptic device workspace. */
hduVector3Dd gCenter;

/* Current button state. */
int btn[ 3 ] = { 0, 0, 0 };        
/* Current mouse position. */
int mouse_x, mouse_y;            

/* Used for dynamics simulation. The dynamics simulation runs
   at the servoloop rate. */
ContactModel* gContactModelHS;

HHD ghHD = HD_INVALID_HANDLE;
HDSchedulerHandle gSchedulerCallback = HD_INVALID_HANDLE;

void displayFunction(void);
void handleMenu(int);
void handleMouse(int, int, int, int);
void initGlut(int argc, char* argv[]);
void initGraphics(const HDdouble LLB[3], const HDdouble TRF[3]);
void displayVisitorSphere(GLUquadricObj* quadObj, const double position[3]);
void displayFixedSphere(GLUquadricObj* quadObj, const double position[3]);                            
void doGraphicsState();

/******************************************************************************
 Get data modified by the haptics thread, and copy them into data
 that can be used by the graphics thread.
******************************************************************************/
struct Synchronizer  
{
    HHD m_hHD;

    // This will be assigned to the global pointer that is also modified
    // by the haptics thread.
    ContactModel *pContactModelHS;

    // Data from pClientData will be copied and assigned in the following
    // variables in a thread safe fashion.
    hduVector3Dd visitorPosition;
    hduVector3Dd forceOnVisitor;
};


/******************************************************************************
 Client Callback used by the graphics main loop function.
 This callback, which is used in a synchronous fashion,
 has the responsibility of copying (in a thread-safe fashion) data that is
 constantly modified by the haptics thread. 
******************************************************************************/

HDCallbackCode HDCALLBACK GetStateCB(void *pUserData)
{
    Synchronizer *pSynchronizer = static_cast<Synchronizer*>(pUserData);

    // Note: pSynchronizer->pContactModelHS has been assigned to point
    // to the same data structure that is passed in the haptics thread
    // callback. Therefore pSynchronizer->pContactModelHS is 
    // updated in every servoloop tick, and it is not thread safe to
    // access it directly.
    pSynchronizer->visitorPosition = 
        pSynchronizer->pContactModelHS->GetCurrentContactPoint();
                                          
    pSynchronizer->forceOnVisitor =
        pSynchronizer->pContactModelHS->GetCurrentForceOnVisitor();
                                          
    // Run this only once.
    return HD_CALLBACK_DONE;
}


/******************************************************************************
 Haptic device record 
******************************************************************************/
struct DeviceDisplayState
{
    HHD m_hHD;
    double position[3];
    double force[3];
};

/******************************************************************************
 Query haptic device state: position, force, etc.
******************************************************************************/
HDCallbackCode HDCALLBACK DeviceStateCallback(void *pUserData)
{
    DeviceDisplayState *pDisplayState = 
        static_cast<DeviceDisplayState *>(pUserData);

    HHD hHD = hdGetCurrentDevice();
    hdBeginFrame(hHD);
    hdGetDoublev(HD_CURRENT_POSITION, pDisplayState->position);
    hdGetDoublev(HD_CURRENT_FORCE, pDisplayState->force);
    hdEndFrame(hHD);

    return HD_CALLBACK_DONE;
}


/******************************************************************************
 Graphics main loop function.
******************************************************************************/
void displayFunction(void)
{
    glPushMatrix();


    doGraphicsState();

    // The synchronizer has the responsibility of getting a snapshot of data
    // from the haptics thread, in a thread safe fashion.
    Synchronizer synchronizer;
    
    // The synchronizer object has the same pointer to the
    // haptic state, as the one that is used in the haptics loop.
    // However, the graphics loop (i.e. this function) cannot
    // use this pointer directly, because the haptics loop may
    // be in the process of modifying the data of that pointer.
    // The thread safe way to access the haptic state is by 
    // defining a synchronous callback, as follows.
    synchronizer.pContactModelHS = gContactModelHS;

    // This callback will be executed immediately, in a thread
    // safe fashion.
    hdScheduleSynchronous(GetStateCB,
                          &synchronizer,
                          HD_DEFAULT_SCHEDULER_PRIORITY);


    GLUquadricObj* quadObj = gluNewQuadric();

    // Sort the objects, so as to draw the object that is "behind"
    // first.
    if(synchronizer.visitorPosition[2] < gCenter[2])
    {
        displayVisitorSphere(quadObj, synchronizer.visitorPosition);
        displayFixedSphere(quadObj, gCenter);
    }
    else
    {
        displayFixedSphere(quadObj, gCenter);
        displayVisitorSphere(quadObj, synchronizer.visitorPosition);
    }

    gluDeleteQuadric(quadObj);
    glEnable(GL_LIGHTING);
  
    glPopMatrix();
    glutSwapBuffers();            
}                    


/******************************************************************************
 Popup menu handler.
******************************************************************************/
void handleMenu(int ID)
{
    switch(ID) {
        case 0:
            exit(0);
            break;
    }
}                    

/******************************************************************************
 Mouse events handler.
******************************************************************************/
void handleMouse(int b, int s, int x, int y)
{
    if (s == GLUT_DOWN) {    
        btn[ b ] = 1;
    } else {
        btn[ b ] = 0;
    }

    mouse_x = x;
    mouse_y = glutGet(GLUT_WINDOW_HEIGHT) - y;
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
 Main callback that runs the dynamics simulation within the haptics loop.
******************************************************************************/
HDCallbackCode HDCALLBACK ContactCB(void *data)
{
    ContactModel *pContactModelHS = static_cast<ContactModel*>(data);

    HHD hHD = hdGetCurrentDevice();

    hdBeginFrame(hHD);

    hduVector3Dd newEffectorPosition;
    hdGetDoublev(HD_CURRENT_POSITION,newEffectorPosition);

    // Given the new master position, run the dynamics simulation and
    // update the slave position.
    pContactModelHS->UpdateEffectorPosition(newEffectorPosition);

    hduVector3Dd forceVec;
    forceVec = pContactModelHS->GetCurrentForceOnVisitor();

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

/******************************************************************************
 Schedules the contact simulation callback.
******************************************************************************/
void DefineForceField()
{
    DeviceDisplayState initialMasterState;
    hdScheduleSynchronous(DeviceStateCallback, &initialMasterState,
                          HD_MAX_SCHEDULER_PRIORITY);

    // Initialize the slave position, relative to the master.
    hduVector3Dd visitorLocation(2*VISITOR_SPHERE_RADIUS + FIXED_SPHERE_RADIUS,
                                 2*VISITOR_SPHERE_RADIUS + FIXED_SPHERE_RADIUS,
                                 0.0);


    gContactModelHS = new ContactModel(FIXED_SPHERE_RADIUS,
                                       gCenter,
                                       VISITOR_SPHERE_RADIUS,
                                       visitorLocation);
    
    gSchedulerCallback = hdScheduleAsynchronous(ContactCB,
                                                gContactModelHS, 
                                                HD_DEFAULT_SCHEDULER_PRIORITY);
}

/******************************************************************************
 This handler gets called when the process is exiting. Ensure that HDAPI is
 properly shutdown.
******************************************************************************/
void exitHandler()
{
    hdStopScheduler();

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
    // Initialize the device.  This needs to be called before any actions on the
    // device.
    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        return -1;
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
        return -1;
    }
    
    initGlut(argc, argv);

    HDdouble maxWorkspace[6];
    hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);
    
    // Low, Left, Back point of device workspace.
    hduVector3Dd LLB(maxWorkspace[0], maxWorkspace[1], maxWorkspace[2]);
    // Top, Right, Front point of device workspace.
    hduVector3Dd TRF(maxWorkspace[3], maxWorkspace[4], maxWorkspace[5]);
    initGraphics(LLB, TRF);

    gCenter = (LLB + TRF)/2.0;

    DefineForceField();

    // Enter GLUT main loop.
    glutMainLoop();            

    // HDAPI cleanup is managed by the atexit handler.
    printf("Done\n");
    return 0;
}

/******************************************************************************/

