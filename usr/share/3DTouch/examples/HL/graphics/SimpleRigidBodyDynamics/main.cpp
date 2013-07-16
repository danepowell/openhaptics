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

  Main program for rigid body simulation.

*******************************************************************************/

#include "SimpleRigidBodyDynamicsAfx.h"
#pragma warning( disable : 4786 )  // identifier was truncated to '255' 
                                   // the "you are using STL" warning

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

#include "DynamicsWorld.h"
#include "draw_string.h"
#include "TestScenes.h"

#include <HL/hl.h>
#include <HDU/hdu.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>

const int kMaxStepSizeMs = 33;

enum 
{  
    DEFAULT_WINDOW_WIDTH = 640,
    DEFAULT_WINDOW_HEIGHT = 480 
};
                
GLint mWindW=DEFAULT_WINDOW_WIDTH;
GLint mWindH=DEFAULT_WINDOW_HEIGHT;

char mApplicationName[] = "Simple Rigid Body Dynamics";

DynamicsWorld *mWorld = 0;

float mManualTimeStep = 0.033; // time to increment by when stepping manually
bool  mPause = false; // pause the simulation

HHD hHD = HD_INVALID_HANDLE;
HHLRC hHLRC = NULL;
double mCursorScale;
GLuint mCursorDisplayList;

int nHapticDeviceDOFInput;
int nHapticDeviceDOFOutput;

void SpawnWindow(void);
void SetGraphicsOptions(void);
void InitHL(void);
void SetupCallbacks(void);
void CreateMenus(void);

void DoViewingTransformation(void);
void DoProjectionTransformation(void);
void UpdateHapticMapping(void);

void Display(void);
void DisplayInfo(void);
float DetermineFPS(void);
void RedrawCursor(void);

void Reshape(int width, int height);
void Idle(void);

void AdvanceTime(double dt);
double GetTimeChange(void);

void HandleMouseButton(int button, int state, int x, int y);
void HandleMouseMotion(int x, int y);
void HandleKeyboard(unsigned char key, int x, int y);
//void HandleSpecialKeys(int key, int x, int y);
void HandleMenuCommand(int option);
void exitHandler();

int main(int argc, char **argv)
{
    // initialize GLUT
    glutInit(&argc, argv);
        
    InitHL();

    testVertexFaceCollision();

        // spawn the main window
    SpawnWindow();
        
    /* The GLUT main loop won't return control, so we need to perform cleanup
       using an exit handler. */
    atexit(exitHandler);

    glutMainLoop();

    return 0;
}


/*
    Function:       InitHL
    Usage:          InitHL();
    ---------------------------------------------------------------------------
    Initializes haptic device and haptic rendering library.
                
*/
void InitHL()
{
    HDErrorInfo error;
    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(1);
    }

    printf("Feel and push the blocks as they collide with each other.\n\n");
    printf("Touch a block and hold the stylus button to attach the haptic device\n");
    printf("to the block such that the device 'grabs' the block via a spring/damper system.\n\n");
    printf("Right click in the graphics window to Bring up different scenes and options.\n");

    // Create a haptic context for the device. The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC); 

    // front face touchable only - no need to feel back faces
    hlTouchableFace(HL_FRONT);

    // get some info about device so we can customize haptics
    // for the device
    hdGetIntegerv(HD_INPUT_DOF, &nHapticDeviceDOFInput);
    hdGetIntegerv(HD_OUTPUT_DOF, &nHapticDeviceDOFOutput);
}

/*******************************************************************************
 Cleanup
*******************************************************************************/
void exitHandler()
{
    if (mWorld != NULL)
    {
        delete mWorld;
    }

    // free up the haptic rendering context
    hlMakeCurrent(NULL);
    if (hHLRC != NULL)
    {
        hlDeleteContext(hHLRC);
    }

    // free up the haptic device
    if (hHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(hHD);
    }
}

void SpawnWindow(void)
{
    // display modes: 24 BIT, double buffer mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        
    glutInitWindowSize(mWindW,mWindH);
    glutCreateWindow(mApplicationName);

    // clear the display
    //glClearColor(1.0,1.0,1.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetGraphicsOptions();
    SetupCallbacks();
    CreateMenus();
}

void SetGraphicsOptions(void)
{
    /* set default attributes */
    //glPolygonMode(GL_BACK,GL_POINT);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glLineWidth(3);
    glPointSize(4);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
        
    GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    //GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat matShininess[] = { 50.0 };
    GLfloat lightPosition[] = { -200.0, 200.0, 100.0, 0.0 };
    //GLfloat lightPosition[] = { 0.0, 400.0, 0.0, 0.0 };
    GLfloat whiteLight[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lModelAmbient[] = { 0.1, 0.1, 0.1, 0.1 };
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lModelAmbient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void SetupCallbacks(void)
{       
    // Window redisplay and reshape event callbacks.
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape); 

    // Setup mouse handler callbacks.
    glutMotionFunc(HandleMouseMotion);
    glutMouseFunc(HandleMouseButton);
        
    glutKeyboardFunc(HandleKeyboard);
    //glutSpecialFunc(HandleSpecialKeys);
        
    glutIdleFunc(Idle);
}

/*
    Function:       CreateMenus
    Usage:          CreateMenus();
    ---------------------------------------------------------------------------
    Creates the menu which popups when the right mouse is clicked.
                
*/
void CreateMenus(void)
{
    glutCreateMenu(HandleMenuCommand);
        
    glutAddMenuEntry("Pause (space)", ' ');
    glutAddMenuEntry("Manual Advance (m)", 'm');
    glutAddMenuEntry("-", 0);

    glutAddMenuEntry("Test Some Collisions (0)", '0');
    glutAddMenuEntry("Test Vertex-Face Collision (1)", '1');
    glutAddMenuEntry("Test Edge-Edge Collision (2)", '2');
    glutAddMenuEntry("Test Big-Small Face Collision (3)", '3');
    glutAddMenuEntry("Test Multi-Box Drop (4)", '4');
    glutAddMenuEntry("Test Spins (5)", '5');
    glutAddMenuEntry("Test Dominos (6)", '6');
    glutAddMenuEntry("Test Cube Mahem (7)", '7');
    glutAddMenuEntry("Test Cube Tower (8)", '8');
    glutAddMenuEntry("Test Catapult (9)", '9');
    glutAddMenuEntry("-", 0);

    glutAddMenuEntry("Toggle Draw Witnesses (w)", 'w');
    // glutAddMenuEntry("Toggle Draw Normals", 'n');
    glutAddMenuEntry("-", 0);

    glutAddMenuEntry("Quit", 'q');
        
#ifdef macintosh
    glutAttachMenuName(GLUT_RIGHT_BUTTON, "Options");
#else
    glutAttachMenu(GLUT_RIGHT_BUTTON);
#endif
}

/* Handle menu commands. */
void HandleMenuCommand(int option)
{
    switch(option)
    {
        case '0':
            testBasicCollisions();
            break;

        case '1':
            testVertexFaceCollision();
            break;

        case '2':
            testEdgeEdgeCollision();
            break;

        case '3':
            testBigSmallFaceCollision();
            break;

        case '4':
            testMultiBoxDrop();
            break;

        case '5':
            testSpins();
            break;

        case '6':
            testDominos();
            break;

        case '7':
            testCubeMahem();
            break;

        case '8':
            testCubeTower();
            break;
                
        case '9':
        case 'c':
            testCatapult();
            break;
                
        case 'r':
            testReboundEdgeEdgeCollision();
            break;

        case 'd':
            testRolledBoxDrop();
            break;

        case 'i':
            testEdgePlaneInvalidateCollision();
            break;

        case 'w':
            mWorld->setDrawWitnesses(!mWorld->getDrawWitnesses());
            break;

        case 'm':
            if (mPause)
                AdvanceTime(mManualTimeStep);
            break;
                        
        case ' ':
            mPause = !mPause;
            if (!mPause)
                glutPostRedisplay();
            break;

        case 'q':  // quit
            exit(0);
            break;

        default:
            //printf("Invalid menu choice: %c", option);
            break;
    }
}


/*
    Function:       HandleKeyboard
    Usage:          HandleKeyboard(key, x, y)
    ---------------------------------------------------------------------------
    Handles some useful keyboard shortcuts.
*/
void HandleKeyboard(unsigned char key, int x, int y)
{
    if (key == 27)
        key = 'q';

    HandleMenuCommand(key);
}

/*
    Function:       HandleSpecialKeys
    Usage:          HandleSpecialKeys(key, x, y);
    ---------------------------------------------------------------------------
    Handles special keys, ie arrow keys.  Arrow keys perform similar functions
    to left mouse dragging.
*/
void HandleSpecialKeys(int key, int x, int y)
{
    if((key == GLUT_KEY_LEFT || key == GLUT_KEY_UP ||
        key == GLUT_KEY_RIGHT || key == GLUT_KEY_DOWN))
    {
        float dx, dy;  // ammount to move in x,y
                
        switch(key)
        {
            case GLUT_KEY_LEFT:
                dx = -1;
                dy = 0;
                break;
            case GLUT_KEY_RIGHT:
                dx = 1;
                dy = 0;
                break;
            case GLUT_KEY_UP:
                dx = 0;
                dy = 1;
                break;
            case GLUT_KEY_DOWN:
                dx = 0;
                dy = -1;
                break;
            default:
                break;
        }                               
                
        if(glutGetModifiers() & GLUT_ACTIVE_SHIFT)
        {
            dx *= 10;
            dy *= 10;
        }
    }
}


/*
    Function:       HandleMouseButton
    Usage:          HandleMouseButton(button, state, x, y);
    ---------------------------------------------------------------------------
    mouse press/release handler
    
       left button:         pull at body
       middle button:       unused
       right button:        unused (mapped to pull-down menu)
*/
void HandleMouseButton(int button, int state, int x, int y)
{

    if (button == GLUT_LEFT_BUTTON)
    {
        switch (state)
        {
            case GLUT_DOWN:
                mWorld->activateMouseSpring(x, y);
                break;

            case GLUT_UP:
                mWorld->deactivateMouseSpring();
                break;
        }
    }    
}

// Mouse motion callback
void  HandleMouseMotion(int x, int y)
{

    mWorld->moveMouseSpring(x, y);

}

void Display(void)
{
    hlBeginFrame();
    hlCheckEvents();

    // Clear the display.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mWorld->drawWorld();

    if (mWorld->getHapticMode() == DynamicsWorld::TOUCH_OBJECTS)
    {
        // Touching objects - render world haptically.
        mWorld->drawWorldHaptics();

        // Draw the 3D cursor.
        RedrawCursor();
    }
    else
    {
        // Controlling an object, don't draw cursor, update proxy 
        // position to that of controlled object.

        // Grab offsets of body transform to initial haptic device
        // transform we saved off in button down event callback.  Use these
        // offsets in setting proxy so that we get force/torque to pull us
        // towards offset body not actual body which would give us a big 
        // impulse when we first grabbed the object if the haptic device and 
        // body transforms did not match.
        hduVector3Dd linOffset;
        hduQuaternion angOffset;
        mWorld->getHapticControlObjectOffset(linOffset, angOffset);

        // Set proxy position to offset position.
        hduVector3Dd offsetPos =  
            mWorld->getBodyPosition(mWorld->getHapticControlObject())
            - linOffset;
        hlProxydv(HL_PROXY_POSITION, offsetPos);
        hlProxyf(HL_STIFFNESS, .3);
        hlProxyf(HL_DAMPING, 0);
        
        // Set proxy rotation to offset rotation (this doesn't do anything
        // if you aren't using a 6DOF haptic device).
        hduQuaternion offsetRot = 
            mWorld->getBodyOrientation(mWorld->getHapticControlObject()) 
            * angOffset.inverse();
        offsetRot.normalize();
        hlProxydv(HL_PROXY_ROTATION, offsetRot);
    }
        
    hlEndFrame();

    DisplayInfo();

    // Swap the double buffers.
    glutSwapBuffers();  
}

void DisplayInfo(void)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Switch to 2d orthographic mode for drawing text.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, mWindW, mWindH, 0);
    glMatrixMode(GL_MODELVIEW);
        
    glColor3f(1.0, 1.0, 1.0);

    int textRowDown = 0; // Lines of text already drawn downwards from the top.
    int textRowUp = 0; // Lines of text already drawn upwards from the bottom.

    DrawBitmapString(mWindW - 10 * 9, 20 + textRowDown++ * 15, GLUT_BITMAP_9_BY_15, "FPS: %4.1f", DetermineFPS());
    //DrawBitmapString(5, 20 + (textRowDown-1) * 15, GLUT_BITMAP_9_BY_15, "Objects: %d", mWorld->getNumBodies());

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
        
    // Turn depth and lighting back on for 3D rendering.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

float DetermineFPS(void)
{
    static unsigned int     lastMs = glutGet(GLUT_ELAPSED_TIME);
    unsigned int            currentMs = glutGet(GLUT_ELAPSED_TIME);
    unsigned int            framesMs = currentMs - lastMs;
    static unsigned int     framesCounted = 0;
    static float fps = 0;
        
    framesCounted++;

#define DURATION_TO_TIME                1000    // duration in miliseconds for calculating fps

    if(framesMs >= DURATION_TO_TIME)
    {
        fps = (float)1000 * framesCounted / framesMs;
        //printf("last: %ld  curr: %ld\n", g.lastClock, currentClock);
        //printf("clocks: %d  cps: %d  FPS: %f\n", clock(), CLOCKS_PER_SEC, fps);
                
        framesCounted = 0;
        //g.framesTime = 0;

        lastMs = currentMs;
    }

    return fps;
}


/*
    Function:       RedrawCursor
    Usage:          RedrawCursor(); 
    ---------------------------------------------------------------------------
    Displays a cursor using the current haptic device proxy transform and the
    mapping between the workspace and world coordinates
*/
void RedrawCursor(void)
{
    static const double kCursorRadius = 1.0;
    static const int kCursorTess = 8;
    HLdouble proxytransform[16];

    GLUquadricObj *qobj = 0;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    if (!mCursorDisplayList)
    {
        mCursorDisplayList = glGenLists(1);
        glNewList(mCursorDisplayList, GL_COMPILE);
        qobj = gluNewQuadric();
               
        gluSphere(qobj, kCursorRadius, kCursorTess, kCursorTess);
    
        gluDeleteQuadric(qobj);
        glEndList();
    }  

    // Apply the local position/rotation transform of the haptic device proxy.
    hlGetDoublev(HL_PROXY_TRANSFORM, proxytransform);
    glMultMatrixd(proxytransform);
        
    // Apply the local cursor scale factor.
    glScaled(mCursorScale, mCursorScale, mCursorScale);
    
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(mCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}
/*
    Function:       DoViewingTransformation
    Usage:          DoViewingTransformation(offset);
    ---------------------------------------------------------------------------
    Viewing transformation goes from world coordinate to viewing coordinates
    (world to camera).  This part of the transformation pipeline is applied
    every time the scene is rendered.
*/
void DoViewingTransformation(void)
{
    float           target[] = {0, 0, 0};
    float           position[] = {0, 0, 15}; // straight on, by front wall
//      float           position[] = {2, 1, -2}; // up close, at an angle
//      float           position[] = {0, 0.5, 2}; // straight on, up close

    glLoadIdentity();
    gluLookAt(      position[0], position[1], position[2],// position
                    target[0], target[1], target[2],        // target
                    0.0, 1.0, 0.0);                                         // up vector
}

void Reshape(int width, int height)
{
    // Change viewport dimensions.
    glViewport(0, 0, width, height);

    // Remember the width and height.
    mWindW = width;
    mWindH = height;

    DoProjectionTransformation();

    // View matrix transformations to make gluUnProject work.
    DoViewingTransformation();

    // Sync haptics view to graphics.
    UpdateHapticMapping();
}

/*
    Function:       DoProjectionTransformation
    Usage:          DoProjectionTransformation();
    ---------------------------------------------------------------------------
    Projection transformation goes from viewing coordinates to projection
    coordinates (camera to window).  This part of the transformation pipeline
    is static as long as the window remains the same size.  Thus only when
    the window is resized (the Reshape() function) does the projection
    transformation need to be calculated and applied.
*/
void DoProjectionTransformation(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40,      // fov
                   (float)mWindW/mWindH,   // aspect ratio; used to use 1.0
                   8,      // near
                   23);    // far
    glMatrixMode(GL_MODELVIEW);
}


/*
    Function:       UpdateHapticMapping
    Usage:          UpdateHapticMapping(); 
    ---------------------------------------------------------------------------
    Use the current OpenGL viewing transforms to initialize a transform for the
    haptic device workspace so that it's properly mapped to world coordinates.
*/
void UpdateHapticMapping(void)
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
    mCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    const int CURSOR_SIZE_PIXELS = 10;
    mCursorScale *= CURSOR_SIZE_PIXELS;
}

void Idle(void)
{
    if (!mPause)
        AdvanceTime(GetTimeChange());
    else
        glutPostRedisplay();
}

double GetTimeChange(void)
{
    unsigned int    timeMs;
    unsigned int    timeSinceLastMs;
    static unsigned int timeOldMs = 0;
        
    timeMs = glutGet(GLUT_ELAPSED_TIME);

    timeSinceLastMs = timeMs - timeOldMs;

    timeOldMs = timeMs;
        
    if (timeSinceLastMs > kMaxStepSizeMs)
    {
        timeSinceLastMs = kMaxStepSizeMs;
    }
    else if (timeSinceLastMs < 1)
    {
        timeSinceLastMs = 1;
    }

    return timeSinceLastMs/ 1000.;
}

void AdvanceTime(double dt)
{
    static double prevTime = 0;
    double currTime = prevTime + dt;
        
    mWorld->advanceSimulation(prevTime, currTime);
    prevTime = currTime;

    glutPostRedisplay();
}

/******************************************************************************/
