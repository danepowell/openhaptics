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

  This example demonstrates how to manipulate points in the scene.

This source code example may be protected by the following U.S. Patents:  6,671,651,
and may be protected by other U.S. patents or foreign patents. Other patents pending.

*******************************************************************************/

#include "PointManipulationAfx.h"

#include <stdlib.h>
#include <cmath>
#include <cassert>

#include <HDU/hduMath.h>
#include <HDU/hduMatrix.h>

#include "PointManager.h"
#include "HapticDeviceManager.h"
#include <iostream>

/* Function prototypes. */
void glutDisplay();
void glutReshape(int width, int height);
void glutIdle(); 
void glutMouse(int button, int state, int x, int y);
void glutMotion(int x, int y);
void glutMenu(int value);

void initGL();
void initScene();
void drawScene();
void updateCamera();

void exitHandler();

static IPointManager *gPointManager = 0;
static IHapticDeviceManager *gHapticManager = 0;

static hduVector3Dd gCameraPosWC;
static int gWindowWidth, gWindowHeight;

/* Variables used by the trackball emulation. */
static hduMatrix gCameraRotation;
static double gCameraScale = 1.0;
static bool gIsRotatingCamera = false;
static bool gIsScalingCamera = false;
static int gLastMouseX, gLastMouseY;

static const double kPI = 3.1415926535897932384626433832795;

/*******************************************************************************
 Main function
*******************************************************************************/
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);

    glutCreateWindow("Point Manipulation Example");

    /* Set glut callback functions. */
    glutDisplayFunc(glutDisplay);
    glutReshapeFunc(glutReshape);
    glutIdleFunc(glutIdle);
    glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
    
    glutCreateMenu(glutMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutAddMenuEntry("Free Manipulation",
                     IHapticDeviceManager::FREE_MANIPULATION);
    glutAddMenuEntry("Spring Manipulation",
                     IHapticDeviceManager::SPRING_MANIPULATION);
    glutAddMenuEntry("Friction Manipulation",
                     IHapticDeviceManager::FRICTION_MANIPULATION);    
    glutAddMenuEntry("Plane Manipulation",
                     IHapticDeviceManager::PLANE_MANIPULATION);
    glutAddMenuEntry("Quit", 5);

    /* The GLUT main loop won't return control, so we need to perform cleanup
       using an exit handler. */
    atexit(exitHandler);

    initScene();

    std::cout << "This example demonstrates point snapping \
and manipulation." << std::endl;
    std::cout << "Use the right mouse button to select \
different manipulation styles." << std::endl;

    glutMainLoop();

    return 0;
}

/*******************************************************************************
 GLUT callback for redrawing the view. Use this as an opportunity to perform
 graphics rate processing, like updating the graphic state of the haptic 
 manager.
*******************************************************************************/
void glutDisplay()
{    
    gHapticManager->updateState();

    drawScene();
    glutSwapBuffers();
}

/*******************************************************************************
 GLUT callback for reshaping the window. This is the main place where the 
 viewing and workspace transforms get initialized.
*******************************************************************************/
void glutReshape(int width, int height)
{
    static const double kFovY = 40;
    static const double kCanonicalSphereRadius = sqrt(3.0);

    glViewport(0, 0, width, height);
    gWindowWidth = width;
    gWindowHeight = height;

    /* Compute the viewing parameters based on a fixed fov and viewing
       sphere enclosing a canonical box centered at the origin. */

    double nearDist = kCanonicalSphereRadius / tan((kFovY / 2.0) * kPI / 180.0);
    double farDist = nearDist + 2.0 * kCanonicalSphereRadius;
    double aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);

    /* Place the camera down the Z axis looking at the origin. */
    gCameraPosWC[0] = 0;
    gCameraPosWC[1] = 0;
    gCameraPosWC[2] = nearDist + kCanonicalSphereRadius;
 
    updateCamera();
}

/*******************************************************************************
 GLUT callback for idle state. Use this as an opportunity to request a redraw.
*******************************************************************************/
void glutIdle()
{
    glutPostRedisplay();
}

/*******************************************************************************
 Initialize the scene. Handle initializing both OpenGL and HDAPI. Create
 the singleton instances of the IPointManager and IHapticDeviceManager.
*******************************************************************************/
void initScene()
{
    initGL();

    gPointManager = IPointManager::create();
    gPointManager->setup();

    gHapticManager = IHapticDeviceManager::create();
    gHapticManager->setup(gPointManager);
}

/*******************************************************************************
 Cleanup and destroy the singleton managers
*******************************************************************************/
void exitHandler()
{
    if (gHapticManager)
    {
        gHapticManager->cleanup();
        IHapticDeviceManager::destroy(gHapticManager);
    }

    if (gPointManager)
    {
        gPointManager->cleanup();
        IPointManager::destroy(gPointManager);
    }
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
 Set the modelview transform from scratch. Apply the current view orientation
 and scale. Also update the PointManager and HapticManager, since they have
 display transforms that are view dependent.
*******************************************************************************/
void updateCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(gCameraPosWC[0], gCameraPosWC[1], gCameraPosWC[2],
              0, 0, 0,
              0, 1, 0);
    
    glMultMatrixd(gCameraRotation);
    glScaled(gCameraScale, gCameraScale, gCameraScale);

    gPointManager->updatePointSize();
    gHapticManager->updateWorkspace();

    glutPostRedisplay();
}

/******************************************************************************
 The main routine for displaying the scene. The drawing is actually handled
 by the IPointManager and IHapticDeviceManager instances.
******************************************************************************/
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        

    gPointManager->drawPoints();
    gHapticManager->drawCursor();
}

/******************************************************************************
 GLUT callback for responding to mouse button presses. Detect whether to
 initiate a view rotation or view scale.
******************************************************************************/
void glutMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
        {
            gIsScalingCamera = true;
        }
        else
        {
            gIsRotatingCamera = true;
        }

        gLastMouseX = x;
        gLastMouseY = y;
    }
    else
    {
        gIsRotatingCamera = false;
        gIsScalingCamera = false;
    }
}

/******************************************************************************* 
 This routine is used by the view rotation code for simulating a virtual
 trackball. This math computes the z height for a 2D projection onto the
 surface of a 2.5D sphere. When the input point is near the center of the
 sphere, it computes the actual sphere intersection in Z. When you move towards
 the outside of the sphere, it will solve for a hyperbolic projection, so that
 you still get a meaningful answer.
*******************************************************************************/
double projectToTrackball(double radius, double x, double y)
{
    static const double kUnitSphereRadius2D = sqrt(2.0);
    double z;

    double dist = sqrt(x * x + y * y);
    if (dist < radius * kUnitSphereRadius2D / 2.0)
    {
        /* Solve for sphere case. */
        z = sqrt(radius * radius - dist * dist);
    }
    else
    {
        /* Solve for hyperbolic sheet case. */
        double t = radius / kUnitSphereRadius2D;
        z = t * t / dist;
    }

    return z;
}

/*******************************************************************************
 GLUT callback for mouse motion, which is used for controlling the view
 rotation and scaling.
*******************************************************************************/
void glutMotion(int x, int y)
{
    if (gIsRotatingCamera)
    {
        static const double kTrackBallRadius = 0.8;   

        hduVector3Dd lastPos;
        lastPos[0] = gLastMouseX * 2.0 / gWindowWidth - 1.0;
        lastPos[1] = (gWindowHeight - gLastMouseY) * 2.0 / gWindowHeight - 1.0;
        lastPos[2] = projectToTrackball(kTrackBallRadius, lastPos[0], lastPos[1]);

        hduVector3Dd currPos;
        currPos[0] = x * 2.0 / gWindowWidth - 1.0;
        currPos[1] = (gWindowHeight - y) * 2.0 / gWindowHeight - 1.0;
        currPos[2] = projectToTrackball(kTrackBallRadius, currPos[0], currPos[1]);

        currPos.normalize();
        lastPos.normalize();

        hduVector3Dd rotateVec = lastPos.crossProduct(currPos);
        
        double rotateAngle = asin(rotateVec.magnitude());
        if (!hduIsEqual(rotateAngle, 0.0, DBL_EPSILON))
        {
            hduMatrix deltaRotation = hduMatrix::createRotation(
                rotateVec, rotateAngle);            
            gCameraRotation.multRight(deltaRotation);
        
            updateCamera();
        }
    }
    else if (gIsScalingCamera)
    {
        float y1 = gWindowHeight - gLastMouseY;
        float y2 = gWindowHeight - y;

        gCameraScale *= 1 + (y1 - y2) / gWindowHeight;  

        updateCamera();
    }

    gLastMouseX = x;
    gLastMouseY = y;
}

/*******************************************************************************
 Handle setting the current manipulation style of the HapticManager
*******************************************************************************/
void glutMenu(int value)
{
    switch (value)
    {   
        case IHapticDeviceManager::FREE_MANIPULATION:
        case IHapticDeviceManager::FRICTION_MANIPULATION:
        case IHapticDeviceManager::SPRING_MANIPULATION:
        case IHapticDeviceManager::PLANE_MANIPULATION:
            gHapticManager->setManipulationStyle(value);
            break;
        case 5:
            exit(0);
            break;
        default:
            assert(!"Unimplemented");
    }
}

/******************************************************************************/
