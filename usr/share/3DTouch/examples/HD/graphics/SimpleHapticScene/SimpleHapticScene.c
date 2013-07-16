/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  SimpleHapticScene.c

Description: 

  This example demonstrates some of the basic building blocks needed for a
  graphics + haptics application. One of the most important aspects is that
  it demonstrates how to map the workspace of the haptic device to
  the view and display a 3D cursor. It also shows how to properly sample and
  display state in the graphics thread that's changing in the haptic thread.

******************************************************************************/

#include <stdio.h>
#include <math.h>

#include <assert.h>

#include <HD/hd.h>
#include <HDU/hdu.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

static HHD ghHD = HD_INVALID_HANDLE;
static HDSchedulerHandle hUpdateDeviceCallback = HD_INVALID_HANDLE;

/* Ttransform that maps the haptic device workspace to world
   coordinates based on the current camera view. */
static HDdouble workspacemodel[16];

static double kCursorScreenSize = 20.0;
static GLuint gCursorDisplayList = 0;
static double gCursorScale = 1.0;

static hduVector3Dd gAnchorPosition;
static HDboolean gIsAnchorActive = HD_FALSE;

typedef struct
{
    hduVector3Dd position;
    HDdouble transform[16];
    hduVector3Dd anchor;
    HDboolean isAnchorActive;
} HapticDisplayState;

/* Function prototypes. */
void glutDisplay(void);
void glutReshape(int width, int height);
void glutIdle(void);
void glutMenu(int);  

int exitHandler(void);

void initGL();
void initHD();
void initScene();
void drawScene();
void drawCursor(const HapticDisplayState *pState);
void drawAnchor(const HapticDisplayState *pState);
void drawWorkspace();
void updateCamera();
void updateWorkspace();

HDCallbackCode HDCALLBACK touchScene(void *pUserData);
HDCallbackCode HDCALLBACK copyHapticDisplayState(void *pUserData);

/*******************************************************************************
 Initializes GLUT for displaying a simple haptic scene.
*******************************************************************************/
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutCreateWindow("Simple Haptic Scene");

    /* Set glut callback functions. */
    glutDisplayFunc(glutDisplay);
    glutReshapeFunc(glutReshape);
    glutIdleFunc(glutIdle);
    
    glutCreateMenu(glutMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    /* Provide a cleanup routine for handling application exit. */
    atexit(exitHandler);

    initScene();

    glutMainLoop();

    return 0;
}

/*******************************************************************************
 GLUT callback for redrawing the view 
*******************************************************************************/
void glutDisplay()
{    
    drawScene();
    glutSwapBuffers();
}

/*******************************************************************************
 GLUT callback for reshaping the window. This is the main place where the 
 viewing and workspace transforms get initialized.
*******************************************************************************/
void glutReshape(int width, int height)
{
    static const double kPI = 3.1415926535897932384626433832795;
    static const double kFovY = 40;

    double nearDist, farDist, aspect;
    
    //Check for screen size before updating..
    if (width || height)
    {
    glViewport(0, 0, width, height);

    /* Compute the viewing parameters based on a fixed fov and viewing
       a canonical box centered at the origin. */

    nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180.0);
    farDist = nearDist + 2.0;
    aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);

    /* Place the camera down the Z axis looking at the origin. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(0, 0, nearDist + 1.0,
              0, 0, 0,
              0, 1, 0);
    
    updateWorkspace();
    }
}

/*******************************************************************************
 GLUT callback for idle state. Use this as an opportunity to request a redraw.
*******************************************************************************/
void glutIdle()
{
    glutPostRedisplay();
}

/******************************************************************************
 Popup menu handler
******************************************************************************/
void glutMenu(int ID)
{
    switch(ID) {
        case 0:
            exit(0);
            break;
    }
}

/*******************************************************************************
 This handler will get called when the application is exiting. This is our
 opportunity to cleanly shutdown the HD API.
*******************************************************************************/
int exitHandler()
{
    hdStopScheduler();
    hdUnschedule(hUpdateDeviceCallback);

    if (ghHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(ghHD);
        ghHD = HD_INVALID_HANDLE;
    }

    return 0;    
}

/*******************************************************************************
 Initialize the scene.  Handles initializing both OpenGL and HDAPI.
*******************************************************************************/
void initScene()
{
    initGL();
    initHD();
}

/*******************************************************************************
 Setup general OpenGL rendering properties, like lights, depth buffering, etc.
*******************************************************************************/
void initGL()
{
    static const GLfloat light_model_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    static const GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.9f, 0.9f};   
    static const GLfloat light0_direction[] = {0.0f, -0.4f, 1.0f, 0.0f};    
    
    /* Enable depth buffering for hidden surface removal. */
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    
    /* Cull back faces. */
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    /* Other misc features. */
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
    glEnable(GL_LIGHT0);   
}

/*******************************************************************************
 Initializes the HDAPI.  This involves initing a device configuration, enabling
 forces, and scheduling a haptic thread callback for servicing the device.
*******************************************************************************/
void initHD()
{
    HDErrorInfo error;
    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError())) 
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getchar();
        exit(-1);
    }


    hdEnable(HD_FORCE_OUTPUT);
        
    hUpdateDeviceCallback = hdScheduleAsynchronous(
        touchScene, 0, HD_MAX_SCHEDULER_PRIORITY);

    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start the scheduler");
        exit(-1);
    }
}

/*******************************************************************************
 Uses the current OpenGL viewing transforms to initialize a transform for the
 haptic device workspace so that it's properly mapped to world coordinates.
*******************************************************************************/
void updateWorkspace()
{
    HDdouble screenTworkspace;
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    /* Compute the transform for going from device coordinates to world
       coordinates based on the current viewing transforms. */
    hduMapWorkspaceModel(modelview, projection, workspacemodel);

    /* Compute the scale needed to display the cursor of a particular size
       in screen coordinates. */
    screenTworkspace = hduScreenToWorkspaceScale(
        modelview, projection, viewport, workspacemodel);

    gCursorScale = kCursorScreenSize * screenTworkspace;
}

/*******************************************************************************
 The main routine for displaying the scene.  Gets the latest snapshot of state
 from the haptic thread and uses it for displaying a 3D cursor.  Also, draws the
 anchor visual if the anchor is presently active.
*******************************************************************************/
void drawScene()
{
    HapticDisplayState state;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    
    /* Obtain a thread-safe copy of the current haptic display state. */
    hdScheduleSynchronous(copyHapticDisplayState, &state,
                          HD_DEFAULT_SCHEDULER_PRIORITY);
  
    drawCursor(&state);

    /* Check if the anchor is presently active and draw a visual for it.
       Note that the anchor state is captured as part of the snapshot,
       so that we can query its state in a thread-safe manner. */
    if (state.isAnchorActive)
    {
        drawAnchor(&state);
    }

    /* Use this for debugging purposes to see the mapping of the workspace
       into the view volume. */
    /* drawWorkspace(); */
}

/*******************************************************************************
 Draws a 3D cursor for the haptic device using the current local transform,
 the workspace to world transform and the screen coordinate scale.
*******************************************************************************/
void drawCursor(const HapticDisplayState *pState)
{
    static const double kCursorRadius = 0.5;
    static const double kCursorHeight = 1.5;
    static const int kCursorTess = 15;

    GLUquadricObj *qobj = 0;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    if (!gCursorDisplayList)
    {
        gCursorDisplayList = glGenLists(1);
        glNewList(gCursorDisplayList, GL_COMPILE);
        qobj = gluNewQuadric();
               
        gluCylinder(qobj, 0.0, kCursorRadius, kCursorHeight,
                            kCursorTess, kCursorTess);
            glTranslated(0.0, 0.0, kCursorHeight);
            gluCylinder(qobj, kCursorRadius, 0.0, kCursorHeight / 5.0,
                            kCursorTess, kCursorTess);
    
        gluDeleteQuadric(qobj);
        glEndList();
    }
    
    /* Apply the workspace view transform. */
    glMultMatrixd(workspacemodel);

    /* Apply the local transform of the haptic device. */
    glMultMatrixd(pState->transform);

    /* Apply the local cursor scale factor. */
    glScaled(gCursorScale, gCursorScale, gCursorScale);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(gCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}

/*******************************************************************************
 Draws a line between the device position and the anchor position.  Note
 that the positions of these can be drawn in world coordinates by simply
 applying the workspacemodel transform to the modelview matrix.
*******************************************************************************/
void drawAnchor(const HapticDisplayState *pState)
{
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glPushMatrix();

    /* Apply the workspace view transform. */
    glMultMatrixd(workspacemodel);

    glLineWidth(2.0);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(1.0, 0.0, 0.0);

    glBegin(GL_LINES);
    glVertex3dv(pState->position);
    glVertex3dv(pState->anchor);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

/*******************************************************************************
 Use this scheduler callback to obtain a thread-safe snapshot of the haptic
 device state that we intend on displaying.
*******************************************************************************/
HDCallbackCode HDCALLBACK copyHapticDisplayState(void *pUserData)
{
    HapticDisplayState *pState = (HapticDisplayState *) pUserData;

    hdGetDoublev(HD_CURRENT_POSITION, pState->position);
    hdGetDoublev(HD_CURRENT_TRANSFORM, pState->transform);

    memcpy(pState->anchor, gAnchorPosition, sizeof(hduVector3Dd));
    pState->isAnchorActive = gIsAnchorActive;

    return HD_CALLBACK_DONE;
}

/*******************************************************************************
 This is the main haptic rendering callback.  This callback will render an
 anchored spring force when the user presses the button.
*******************************************************************************/
HDCallbackCode HDCALLBACK touchScene(void *pUserData)
{
    static const HDdouble kAnchorStiffness = 0.2;

    int currentButtons, lastButtons;
    hduVector3Dd position;
    hduVector3Dd force = { 0, 0, 0 };
    HDdouble forceClamp;
    HDErrorInfo error;

    hdBeginFrame(ghHD);
    
    hdGetIntegerv(HD_CURRENT_BUTTONS, &currentButtons);
    hdGetIntegerv(HD_LAST_BUTTONS, &lastButtons);

    /* Detect button state transitions. */
    if ((currentButtons & HD_DEVICE_BUTTON_1) != 0 &&
        (lastButtons & HD_DEVICE_BUTTON_1) == 0)
    {
        gIsAnchorActive = HD_TRUE;
        hdGetDoublev(HD_CURRENT_POSITION, gAnchorPosition);
    }
    else if ((currentButtons & HD_DEVICE_BUTTON_1) == 0 &&
             (lastButtons & HD_DEVICE_BUTTON_1) != 0)
    {
        gIsAnchorActive = HD_FALSE;
    }   

    if (gIsAnchorActive)
    {
        hdGetDoublev(HD_CURRENT_POSITION, position);

        /* Compute force that will attact the device towards the anchor
           position using equation: F = k * (anchor - position). */
        hduVecSubtract(force, gAnchorPosition, position);
        hduVecScaleInPlace(force, kAnchorStiffness);
        
        /* Check if we need to clamp the force. */
        hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &forceClamp);
        if (hduVecMagnitude(force) > forceClamp)
        {
            hduVecNormalizeInPlace(force);
            hduVecScaleInPlace(force, forceClamp);
        }
    }

    hdSetDoublev(HD_CURRENT_FORCE, force);
   
    hdEndFrame(ghHD);

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        if (hduIsForceError(&error))
        {
            /* Disable the anchor following the force error. */
            gIsAnchorActive = HD_FALSE;
        }
        else
        {
            /* This is likely a more serious error, so bail. */
            hduPrintError(stderr, &error, "Error during haptic rendering");
            exit(-1);
        }
    }

    return HD_CALLBACK_CONTINUE;
}

/*******************************************************************************
 This is a debugging routine that draws the workspace that is mapped to the
 view frustum.
*******************************************************************************/
void drawWorkspace()
{
    int i;
    HDdouble usableBounds[6];
    HDdouble maxBounds[6];
    hduVector3Dd minPt;
    hduVector3Dd maxPt;

    static const HDint aLines[12][2] = 
    {
        { 0, 1 }, { 1, 3 }, { 3, 2 }, { 2, 0 },
        { 4, 5 }, { 5, 7 }, { 7, 6 }, { 6, 4 },
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };

    hdGetDoublev(HD_USABLE_WORKSPACE_DIMENSIONS, usableBounds);
    hdGetDoublev(HD_MAX_WORKSPACE_DIMENSIONS, maxBounds);
    hduVecSet(minPt, usableBounds[0], usableBounds[1], usableBounds[2]);
    hduVecSet(maxPt, usableBounds[3], usableBounds[4], maxBounds[5]);

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(1.0, 1.0, 1.0);   
    glLineWidth(2);    

    glPushMatrix();

    /* Apply the workspace view transform. */
    glMultMatrixd(workspacemodel);

    glBegin(GL_LINES);
    for (i = 0; i < 12; i++)
    {
        glVertex3d((aLines[i][0] & (1 << 0)) ? minPt[0] : maxPt[0],
                   (aLines[i][0] & (1 << 1)) ? minPt[1] : maxPt[1],
                   (aLines[i][0] & (1 << 2)) ? minPt[2] : maxPt[2]);

        glVertex3d((aLines[i][1] & (1 << 0)) ? minPt[0] : maxPt[0],
                   (aLines[i][1] & (1 << 1)) ? minPt[1] : maxPt[1],
                   (aLines[i][1] & (1 << 2)) ? minPt[2] : maxPt[2]);
    }
    glEnd();

    glPopMatrix();

    glPopAttrib();
}

/*****************************************************************************/
