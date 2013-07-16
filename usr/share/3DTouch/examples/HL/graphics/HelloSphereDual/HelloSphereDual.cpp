/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  HelloSphere.cpp

Description: 

  This example demonstrates basic haptic rendering of a shape
  using two haptic devices.  It will not run unless you have
  more than one haptic device installed on your computer.

******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#if defined(WIN32)
# include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>

#include <HLU/hlu.h>

static HHD hHD1 = HD_INVALID_HANDLE;
static HHD hHD2 = HD_INVALID_HANDLE;
static HHLRC hHLRC1 = 0;
static HHLRC hHLRC2 = 0;

// shape id for shape we will render haptically
HLuint sphereShapeId1;
HLuint sphereShapeId2;

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
void initHD(HDstring pConfigName, HHD &hHD);
void initHL(HHD hHD, HHLRC &hHLRC, HLuint &shapeId);
void initScene();
void drawSceneHaptics(HLuint shapeId);
void drawSceneGraphics();
void drawCursor(HLfloat color);
void updateWorkspace();

/*******************************************************************************
 Initializes GLUT for displaying a simple haptic scene
*******************************************************************************/
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutCreateWindow("HelloSphere with Dual PHANTOM Example");

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
    hlMakeCurrent(hHLRC1);
    drawSceneHaptics(sphereShapeId1);

    hlMakeCurrent(hHLRC2);
    drawSceneHaptics(sphereShapeId2);

    drawSceneGraphics();

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

    glViewport(0, 0, width, height);

    /* Compute the viewing parameters based on a fixed fov and viewing
     * a canonical box centered at the origin */

    nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180.0);
    farDist = nearDist + 2.0;
    aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);

    /* Place the camera down the Z axis looking at the origin */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(0, 0, nearDist + 1.0,
              0, 0, 0,
              0, 1, 0);
    
    hlMakeCurrent(hHLRC1);
    updateWorkspace();
    hlMakeCurrent(hHLRC2);
    updateWorkspace();    
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
 Initialize the scene. Handle initializing both OpenGL and HL
*******************************************************************************/
void initScene()
{
    initGL();

    // Initialize HDAPI first, so that the device instances exist in the system
    // All device instances need to exist before starting the scheduler,
    // which gets started automatically by the first created context
    initHD("PHANToM 1", hHD1);
    initHD("PHANToM 2", hHD2);

    // Initialize the contexts and give each one a handle to a device instance
    initHL(hHD1, hHLRC1, sphereShapeId1);
    initHL(hHD2, hHLRC2, sphereShapeId2);
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
 Initialize an HD device instance
*******************************************************************************/
void initHD(HDstring pConfigName, HHD &hHD)
{
    HDErrorInfo error;

    hHD = hdInitDevice(pConfigName);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(-1);
    }   

    printf("Found device model: %s / serial number: %s.\n\n", 
        hdGetString(HD_DEVICE_MODEL_TYPE), hdGetString(HD_DEVICE_SERIAL_NUMBER));    
}


/*******************************************************************************
 Initialize an HL rendering context for a particular device instance
*******************************************************************************/
void initHL(HHD hHD, HHLRC &hHLRC, HLuint &shapeId)
{    
    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC);

    // Enable optimization of the viewing parameters when rendering
    // geometry for OpenHaptics
    hlEnable(HL_HAPTIC_CAMERA_VIEW);

    // Specify front face touchability only
    hlTouchableFace(HL_FRONT);

    // generate id's for the three shapes
    shapeId = hlGenShapes(1);
}

/*******************************************************************************
 This handler will get called when the application is exiting.
 Deallocates any state and cleans up.
*******************************************************************************/
void exitHandler()
{
    // deallocate the sphere shape id we reserved in in initHL
    hlDeleteShapes(sphereShapeId1, 1);
    hlDeleteShapes(sphereShapeId2, 1);

    // free up the haptic rendering context
    hlMakeCurrent(NULL);

    if (hHLRC1 != NULL)
    {
        hlDeleteContext(hHLRC1);
    }

    if (hHLRC2 != NULL)
    {
        hlDeleteContext(hHLRC2);
    }

    // free up the haptic device
    if (hHD1 != HD_INVALID_HANDLE)
    {
        hdDisableDevice(hHD1);
    }

    if (hHD2 != HD_INVALID_HANDLE)
    {
        hdDisableDevice(hHD2);
    }
}

/*******************************************************************************
 Use the current OpenGL viewing transforms to initialize a transform for the
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
    
    /* fit haptic workspace to view volume */
    hluFitWorkspace(projection);

    /* compute cursor scale */
    gCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    gCursorScale *= CURSOR_SIZE_PIXELS;
}

/*******************************************************************************
 The main routine for displaying the scene. Get the latest snapshot of state
 from the haptic thread and use it for displaying a 3D cursor.
*******************************************************************************/
void drawSceneGraphics()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);           

    // draw 3D cursor at haptic device position
    hlMakeCurrent(hHLRC1);
    drawCursor(0.5);
    hlMakeCurrent(hHLRC2);
    drawCursor(1.0);

    // draw a sphere using OpenGL
    glutSolidSphere(0.5, 32, 32);
}

/*******************************************************************************
 The main routine for rendering scene haptics. 
 Renders the sphere haptically.
*******************************************************************************/
void drawSceneHaptics(HLuint shapeId)
{    
    // Clear the depth buffer when using a depth buffer shape
    glClear(GL_DEPTH_BUFFER_BIT);   

    // start haptic frame - must do this before rendering any haptic shapes
    hlBeginFrame();
    
    // start a new haptic shape, use the feedback buffer to
    // capture OpenGL geometry for haptic rendering
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, shapeId);

    // use OpenGL commands to create geometry
    glutSolidSphere(0.5, 32, 32);

    // end the shape
    hlEndShape();

    // end the haptic frame
    hlEndFrame();
}


/*******************************************************************************
 Draw a 3D cursor for the haptic device using the current local transform,
 the workspace to world transform and the screen coordinate scale.
*******************************************************************************/
void drawCursor(HLfloat color)
{
    static const double kCursorRadius = 0.5;
    static const double kCursorHeight = 1.5;
    static const int kCursorTess = 15;
    HLdouble proxyxform[16];

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
    
    /* Get the proxy transform in world coordinates */
    hlGetDoublev(HL_PROXY_TRANSFORM, proxyxform);
    glMultMatrixd(proxyxform);

    /* Apply the local cursor scale factor. */
    glScaled(gCursorScale, gCursorScale, gCursorScale);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, color, 1.0);

    glCallList(gCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}

