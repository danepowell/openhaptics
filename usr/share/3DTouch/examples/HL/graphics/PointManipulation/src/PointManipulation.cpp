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
#include <math.h>
#include <assert.h>

#include "ViewManager.h"
#include "PointManager.h"
#include "HapticManager.h"

/* Function prototypes. */
void glutDisplay();
void glutReshape(int width, int height);
void glutIdle(); 
void glutMouse(int button, int state, int x, int y);
void glutMotion(int x, int y);

void initScene();
void drawScene();

void exitHandler();

static IViewManager *gViewManager = 0;
static IPointManager *gPointManager = 0;
static IHapticManager *gHapticManager = 0;

static int gManipPoint = -1;
static bool gIsMouseManipPoint = false;

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
    
    /* The GLUT main loop won't return control, so we need to perform cleanup
       using an exit handler. */
    atexit(exitHandler);

    initScene();    

    glutMainLoop();

    return 0;
}


/*******************************************************************************
 GLUT callback for redrawing the view. Use this as an opportunity to perform
 graphics rate processing.
*******************************************************************************/
void glutDisplay()
{    
    drawScene();
}


/*******************************************************************************
 GLUT callback for reshaping the window. This is the main place where the 
 viewing and workspace transforms get initialized.
*******************************************************************************/
void glutReshape(int width, int height)
{
    gViewManager->reshapeView(width, height);
    gHapticManager->updateWorkspace();
    glutPostRedisplay();
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
 the singleton instances of the IPointManager.
*******************************************************************************/
void initScene()
{
    gViewManager = IViewManager::create();
    gViewManager->setup();

    gPointManager = IPointManager::create();
    gPointManager->setup(gViewManager);

    gHapticManager = IHapticManager::create();
    gHapticManager->setup(gPointManager, gViewManager);
}


/*******************************************************************************
 Cleanup and destroy the singleton managers
*******************************************************************************/
void exitHandler()
{
    if (gHapticManager)
    {
        gHapticManager->cleanup();
        IHapticManager::destroy(gHapticManager);
    }

    if (gPointManager)
    {
        gPointManager->cleanup();
        IPointManager::destroy(gPointManager);
    }

    if (gViewManager)
    {
        gViewManager->cleanup();
        IViewManager::destroy(gViewManager);
    }
}


/******************************************************************************
 The main routine for displaying the scene. The drawing is actually handled
 by the IPointManager instances.
******************************************************************************/
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gPointManager->drawPoints();
    gPointManager->drawLines();

    gHapticManager->feelPoints();

    if (!gHapticManager->isManipulating())
    {
        gHapticManager->drawCursor();
    }

    glutSwapBuffers();
}


/******************************************************************************
 GLUT callback for responding to mouse button presses. Detect whether to
 initiate a point Snapping, view rotation or view scale.
******************************************************************************/
void glutMouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        if (button == GLUT_LEFT_BUTTON)
        {
            int nWindowHeight = gViewManager->getWindowHeight();

            gManipPoint = gPointManager->pickPoint(x, nWindowHeight - y);
            if (gManipPoint != -1)
            {
                gIsMouseManipPoint = true;
            }
        }
        else if (button == GLUT_RIGHT_BUTTON)
        {
            gViewManager->startModifyView(
                IViewManager::MODIFY_VIEW_ROTATE, x, y);        
        }
        else if (button == GLUT_MIDDLE_BUTTON)
        {
            gViewManager->startModifyView(
                IViewManager::MODIFY_VIEW_SCALE, x, y);        
        }
    }
    else
    {
        gIsMouseManipPoint = false;
        gViewManager->stopModifyView();
    }
}


/*******************************************************************************
 GLUT callback for mouse motion, which is used for controlling the view
 rotation and scaling.
*******************************************************************************/
void glutMotion(int x, int y)
{
    if (gIsMouseManipPoint)
    {
        // Translate the point by projecting it to screen space, moving
        // to the new x, y location and unprojecting using the original z
        assert(gManipPoint != -1);
        hduVector3Dd win;
        hduVector3Dd obj = gPointManager->getPointPosition(gManipPoint);
        
        if (gViewManager->toScreen(obj, win))
        {
            win[0] = x;
            win[1] = gViewManager->getWindowHeight() - y;
            if (gViewManager->fromScreen(win, obj))
            {
                gPointManager->setPointPosition(gManipPoint, obj);
                glutPostRedisplay();
            }
        }
    }
    else if (gViewManager->isModifyingView())
    {
        gViewManager->modifyView(x, y);
        glutPostRedisplay();
    }
}

/******************************************************************************/
