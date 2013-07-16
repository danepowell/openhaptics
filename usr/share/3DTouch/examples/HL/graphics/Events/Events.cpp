/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  Events.cpp

Description: 

  This example demonstrates using the event system to detect contact with
  shapes, motion of the haptic device and stylus switch presses.

******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#if defined(WIN32)
#include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#endif

#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>

#include <HLU/hlu.h>

#include <iostream>

static HHD hHD = HD_INVALID_HANDLE;
static HHLRC hHLRC = 0;

/* Shape ids for shapes we will render haptically. */
HLuint gSphereShapeId = 0;
HLuint gTorusShapeId = 0;
HLuint gTeapotShapeId = 0;

#define CURSOR_SIZE_PIXELS 20
static double gCursorScale;
static GLuint gCursorDisplayList = 0;

/* Function prototypes. */
void glutDisplay(void);
void glutReshape(int width, int height);
void glutIdle(void);
void glutMenu(int);

void exitHandler(void);

void initGL();
void initHL();
void initScene();
void drawSceneHaptics();
void drawSceneGraphics();
void drawCursor();
void updateWorkspace();

void HLCALLBACK touchShapeCallback(HLenum event, HLuint object, HLenum thread, 
                                   HLcache *cache, void *userdata)
{
    hduVector3Dd proxy;
    std::cout << "Touched ObjectID: " << object << " at point ";
    hlCacheGetDoublev(cache, HL_PROXY_POSITION, proxy);
    std::cout << proxy << " ";
    HLboolean buttDown;
    hlCacheGetBooleanv(cache, HL_BUTTON1_STATE, &buttDown);
    std::cout << "button 1 " << (buttDown ? "down" : "up") << std::endl;
}

void HLCALLBACK untouchShapeCallback(HLenum event, HLuint object, HLenum thread, 
                                     HLcache *cache, void *userdata)
{
    hduVector3Dd proxy;
    std::cout << "Stopped touching ObjectID: " << object << std::endl;
}

void HLCALLBACK touchSphereCallback(HLenum event, HLuint object, HLenum thread, 
                                    HLcache *cache, void *userdata)
{
    std::cout << "Touched Sphere" << std::endl;
}

void HLCALLBACK touchTorusCallback(HLenum event, HLuint object, HLenum thread, 
                                   HLcache *cache, void *userdata)
{
    std::cout << "Touched Torus" << std::endl;
}

void HLCALLBACK touchTeapotCallback(HLenum event, HLuint object, HLenum thread, 
                                    HLcache *cache, void *userdata)
{
    std::cout << "Touched Teapot" << std::endl;
}

void HLCALLBACK untouchSphereCallback(HLenum event, HLuint object, HLenum thread, 
                                      HLcache *cache, void *userdata)
{
    std::cout << "Stopped Touching Sphere" << std::endl;
}

void HLCALLBACK untouchTorusCallback(HLenum event, HLuint object, HLenum thread, 
                                     HLcache *cache, void *userdata)
{
    std::cout << "Stopped Touching Torus" << std::endl;
}

void HLCALLBACK untouchTeapotCallback(HLenum event, HLuint object, HLenum thread, 
                                      HLcache *cache, void *userdata)
{
    std::cout << "Stopped Touching Teapot" << std::endl;
}

void HLCALLBACK button1DownCallback(HLenum event, HLuint object, HLenum thread, 
                                    HLcache *cache, void *userdata)
{
    std::cout << "Button 1 down" << std::endl;
}

void HLCALLBACK button1UpCallback(HLenum event, HLuint object, HLenum thread, 
                                  HLcache *cache, void *userdata)
{
    std::cout << "Button 1 up" << std::endl;

}

void HLCALLBACK button3DownCallback(HLenum event, HLuint object, HLenum thread, 
                                    HLcache *cache, void *userdata)
{
    std::cout << "Button 3 down" << std::endl;
}

void HLCALLBACK button3UpCallback(HLenum event, HLuint object, HLenum thread, 
                                  HLcache *cache, void *userdata)
{
    std::cout << "Button 3 up" << std::endl;
}

void HLCALLBACK button2DownCallback(HLenum event, HLuint object, HLenum thread, 
                                    HLcache *cache, void *userdata)
{
    std::cout << "Button 2 down" << std::endl;
}

void HLCALLBACK button2UpCallback(HLenum event, HLuint object, HLenum thread, 
                                  HLcache *cache, void *userdata)
{
    std::cout << "Button 2 up" << std::endl;
}

void HLCALLBACK button1DownSphereCallback(HLenum event, HLuint object, HLenum thread, 
                                          HLcache *cache, void *userdata)
{
    std::cout << "Button 1 down on Sphere" << std::endl;
}

void HLCALLBACK button1UpSphereCallback(HLenum event, HLuint object, HLenum thread, 
                                        HLcache *cache, void *userdata)
{
    std::cout << "Button 1 up on Sphere" << std::endl;
}

void HLCALLBACK motionCallback(HLenum event, HLuint object, HLenum thread, 
                               HLcache *cache, void *userdata)
{
    std::cout << "Moving..." << std::endl;
}

void HLCALLBACK motionOnSphereCallback(HLenum event, HLuint object, HLenum thread, 
                                       HLcache *cache, void *userdata)
{
    std::cout << "Moving on sphere..." << std::endl;
}

void HLCALLBACK calibrationCallback(HLenum event, HLuint object, HLenum thread, 
                                    HLcache *cache, void *userdata)
{
    if (event == HL_EVENT_CALIBRATION_UPDATE)
    {
        std::cout << "Device requires calibration update..." << std::endl;
        hlUpdateCalibration();        
    }
    else if (event == HL_EVENT_CALIBRATION_INPUT)
    {
        std::cout << "Device requires calibration.input..." << std::endl;
	    hlUpdateCalibration();
    }
}

/*******************************************************************************
 Initializes GLUT for displaying a simple haptic scene.
*******************************************************************************/
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutCreateWindow("Events Example");

    // Set glut callback functions.
    glutDisplayFunc(glutDisplay);
    glutReshapeFunc(glutReshape);
    glutIdleFunc(glutIdle);
    
    glutCreateMenu(glutMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);    
    
    // Provide a cleanup routine for handling application exit.
    atexit(exitHandler);

    initScene();

    glutMainLoop();

    return 0;
}

/*******************************************************************************
 GLUT callback for redrawing the view.
*******************************************************************************/
void glutDisplay()
{    
    drawSceneHaptics();
    drawSceneGraphics();
    glutSwapBuffers();
}

/*******************************************************************************
 GLUT callback for reshaping the window.  This is the main place where the 
 viewing and workspace transforms get initialized.
*******************************************************************************/
void glutReshape(int width, int height)
{
    static const double kPI = 3.1415926535897932384626433832795;
    static const double kFovY = 40;

    double nearDist, farDist, aspect;

    glViewport(0, 0, width, height);

    // Compute the viewing parameters based on a fixed fov and viewing
    // a canonical box centered at the origin.

    nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180.0);
    farDist = nearDist + 2.0;
    aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);

    // Place the camera down the Z axis looking at the origin.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(0, 0, nearDist + 1.0,
              0, 0, 0,
              0, 1, 0);
    
    updateWorkspace();
}

/*******************************************************************************
 GLUT callback for idle state.  Use this as an opportunity to request a redraw.
 Checks for HLAPI errors that have occurred since the last idle check.
*******************************************************************************/
void glutIdle()
{
    HLerror error;

    while (HL_ERROR(error = hlGetError()))
    {
        fprintf(stderr, "HL Error: %s\n", error.errorCode);
        
        if (error.errorCode == HL_DEVICE_ERROR)
        {
            hduPrintError(stderr, &error.errorInfo,
                "Error during haptic rendering\n");
        }
    }
    
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
 This handler is called when the application is exiting.  Deallocates any state 
 and cleans up.
*******************************************************************************/
void exitHandler()
{
    // Remove event callbacks.
    hlRemoveEventCallback(HL_EVENT_TOUCH, gSphereShapeId, 
        HL_CLIENT_THREAD, touchSphereCallback);
    hlRemoveEventCallback(HL_EVENT_UNTOUCH, gSphereShapeId, 
        HL_CLIENT_THREAD, untouchSphereCallback);
    hlRemoveEventCallback(HL_EVENT_TOUCH, gTorusShapeId, 
        HL_CLIENT_THREAD, touchTorusCallback);
    hlRemoveEventCallback(HL_EVENT_UNTOUCH, gTorusShapeId, 
        HL_CLIENT_THREAD, untouchTorusCallback);
    hlRemoveEventCallback(HL_EVENT_TOUCH, gTeapotShapeId, 
        HL_CLIENT_THREAD, touchTeapotCallback);
    hlRemoveEventCallback(HL_EVENT_UNTOUCH, gTeapotShapeId, 
        HL_CLIENT_THREAD, untouchTeapotCallback);
    hlRemoveEventCallback(HL_EVENT_CALIBRATION_UPDATE, HL_OBJECT_ANY,
        HL_CLIENT_THREAD, calibrationCallback);                       
    hlRemoveEventCallback(HL_EVENT_CALIBRATION_INPUT, HL_OBJECT_ANY,
        HL_CLIENT_THREAD, calibrationCallback);                       

    // Deallocate the shape ids we reserved in initHL.
    hlDeleteShapes(gSphereShapeId, 1);
    hlDeleteShapes(gTorusShapeId, 1);
    hlDeleteShapes(gSphereShapeId, 1);

    // Free up the haptic rendering context.
    hlMakeCurrent(NULL);
    if (hHLRC != NULL)
    {
        hlDeleteContext(hHLRC);
    }

    // Free up the haptic device.
    if (hHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(hHD);
    }
}

/*******************************************************************************
 Initializes the scene.  Handles initializing both OpenGL and HL.
*******************************************************************************/
void initScene()
{
    initGL();
    initHL();
}

/*******************************************************************************
 Sets up general OpenGL rendering properties: lights, depth buffering, etc.
*******************************************************************************/
void initGL()
{
    static const GLfloat light_model_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    static const GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.9f, 0.9f};   
    static const GLfloat light0_direction[] = {0.0f, -0.4f, 1.0f, 0.0f};    
    
    // Enable depth buffering for hidden surface removal.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    
    // Cull back faces.
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    // Setup other misc features.
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    
    // Setup lighting model.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
    glEnable(GL_LIGHT0);   
}

/*******************************************************************************
 Initialize the HDAPI.  This involves initing a device configuration, enabling
 forces, and scheduling a haptic thread callback for servicing the device.
*******************************************************************************/
void initHL()
{
    HDErrorInfo error;

    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(-1);
    }
    
    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC);

    // Enable optimization of the viewing parameters when rendering
    // geometry for OpenHaptics.
    hlEnable(HL_HAPTIC_CAMERA_VIEW);

    // Generate id's for the three shapes.
    gSphereShapeId = hlGenShapes(1);
    gTorusShapeId = hlGenShapes(1);
    gTeapotShapeId = hlGenShapes(1);

    // Setup event callbacks.
    hlAddEventCallback(HL_EVENT_TOUCH, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &touchShapeCallback, NULL);
    hlAddEventCallback(HL_EVENT_UNTOUCH, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &untouchShapeCallback, NULL);
    hlAddEventCallback(HL_EVENT_TOUCH, gSphereShapeId, HL_CLIENT_THREAD, 
                       &touchSphereCallback, NULL);
    hlAddEventCallback(HL_EVENT_UNTOUCH, gSphereShapeId, HL_CLIENT_THREAD, 
                       &untouchSphereCallback, NULL);
    hlAddEventCallback(HL_EVENT_TOUCH, gTorusShapeId, HL_CLIENT_THREAD, 
                       &touchTorusCallback, NULL);
    hlAddEventCallback(HL_EVENT_UNTOUCH, gTorusShapeId, HL_CLIENT_THREAD, 
                       &untouchTorusCallback, NULL);
    hlAddEventCallback(HL_EVENT_TOUCH, gTeapotShapeId, HL_CLIENT_THREAD, 
                       &touchTeapotCallback, NULL);
    hlAddEventCallback(HL_EVENT_UNTOUCH, gTeapotShapeId, HL_CLIENT_THREAD, 
                       &untouchTeapotCallback, NULL);
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &button1DownCallback, NULL);
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &button1UpCallback, NULL);
    hlAddEventCallback(HL_EVENT_2BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &button2DownCallback, NULL);
    hlAddEventCallback(HL_EVENT_2BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &button2UpCallback, NULL);
    hlAddEventCallback(HL_EVENT_3BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &button3DownCallback, NULL);
    hlAddEventCallback(HL_EVENT_3BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &button3UpCallback, NULL);
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, gSphereShapeId, HL_CLIENT_THREAD, 
                       &button1DownSphereCallback, NULL);
    hlAddEventCallback(HL_EVENT_1BUTTONUP, gSphereShapeId, HL_CLIENT_THREAD, 
                       &button1UpSphereCallback, NULL);
    hlAddEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       &motionCallback, NULL);
    hlAddEventCallback(HL_EVENT_MOTION, gSphereShapeId, HL_CLIENT_THREAD, 
                       &motionOnSphereCallback, NULL);
    hlAddEventCallback(HL_EVENT_CALIBRATION_UPDATE, HL_OBJECT_ANY,
                       HL_CLIENT_THREAD, &calibrationCallback, NULL);                       
    hlAddEventCallback(HL_EVENT_CALIBRATION_INPUT, HL_OBJECT_ANY,
                       HL_CLIENT_THREAD, &calibrationCallback, NULL);                       
}

/*******************************************************************************
 Uss the current OpenGL viewing transforms to initialize a transform for the
 haptic device workspace so that it's properly mapped to world coordinates.
*******************************************************************************/
void updateWorkspace()
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();
    
    // Fit haptic workspace to view volume.
    hluFitWorkspace(projection);

    // Compute cursor scale.
    gCursorScale = hluScreenToModelScale(modelview, projection, (HLint*)viewport);
    gCursorScale *= CURSOR_SIZE_PIXELS;
}

void drawSphere()
{
    glPushMatrix();
    glTranslatef(-0.8, -.4, 0);
    glutSolidSphere(0.3, 30, 30);
    glPopMatrix();
}

void drawTorus()
{
    glPushMatrix();
    glTranslatef(0.8, -.4, 0);
    glutSolidTorus(0.1, 0.2, 30, 30);
    glPopMatrix();
}

void drawTeapot()
{
    glDisable(GL_CULL_FACE);
    glPushMatrix();
    glTranslatef(0, 0.6, 0);
    glutSolidTeapot(0.4);
    glPopMatrix();
}

/*******************************************************************************
 The main routine for displaying the scene.  Gets the latest snapshot of state
 from the haptic thread and uses it for displaying a 3D cursor.  Draws the
 anchor visual if the anchor is presently active.
*******************************************************************************/
void drawSceneGraphics()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    
    drawCursor();

    drawSphere();
    drawTorus();
    drawTeapot();
}

/*******************************************************************************
 The main routine for haptically rendering the scene.
*******************************************************************************/
void drawSceneHaptics()
{
    hlBeginFrame();
    
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gSphereShapeId);
    hlTouchableFace(HL_FRONT);
    drawSphere();
    hlEndShape();

    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gTorusShapeId);
    hlTouchableFace(HL_FRONT);
    drawTorus();
    hlEndShape();

    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gTeapotShapeId);
    hlTouchableFace(HL_BACK);
    drawTeapot();
    hlEndShape();

    hlEndFrame();

    // Call any event callbacks that have been triggered.
    hlCheckEvents();
}


/*******************************************************************************
 Draws a 3D cursor for the haptic device using the current local transform,
 the workspace to world transform and the screen coordinate scale.
*******************************************************************************/
void drawCursor()
{
    static const double kCursorRadius = 0.5;
    static const int kCursorTess = 15;
    HLdouble proxytransform[16];

    GLUquadricObj *qobj = 0;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    if (!gCursorDisplayList)
    {
        gCursorDisplayList = glGenLists(1);
        glNewList(gCursorDisplayList, GL_COMPILE);
        qobj = gluNewQuadric();
               
        gluSphere(qobj, kCursorRadius, kCursorTess, kCursorTess);
    
        gluDeleteQuadric(qobj);
        glEndList();
    }  

     // Apply the local position/rotation transform of the haptic device proxy.
    hlGetDoublev(HL_PROXY_TRANSFORM, proxytransform);
    glMultMatrixd(proxytransform);
        
    // Apply the local cursor scale factor.
    glScaled(gCursorScale, gCursorScale, gCursorScale);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(gCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}

/******************************************************************************/
