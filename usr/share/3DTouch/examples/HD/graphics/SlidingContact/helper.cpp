/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  helper.cpp

Description:

  Utilities that set the graphics state. In the initGraphics function we
  demonstrate how use the haptic device reference frame as the arena for
  graphics.

*******************************************************************************/

#include <HDU/hduMatrix.h>
#include "Constants.h"

#include <cstdio>

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

extern void displayFunction(void);
extern void handleMenu(int);
extern void handleIdle(void);
extern void handleMouse(int, int, int, int);

/*******************************************************************************
 GLUT initialization.
*******************************************************************************/
void initGlut(int argc, char* argv[])
{
    glutInit(&argc, argv); /* Initialize GLUT. */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Sphere-sphere Contact Example");

    glutDisplayFunc(displayFunction); /* Setup GLUT callbacks. */
    glutMouseFunc(handleMouse);
    glutIdleFunc(handleIdle);

    glutCreateMenu(handleMenu);    
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/*******************************************************************************    
 Use the haptic device coordinate space as model space for graphics.
 Define orthographic projection to fit it.

 LLB: Low, Left, Back point of device workspace 
 TRF: Top, Right, Front point of device workspace 
*******************************************************************************/
void initGraphics(const HDdouble LLB[3], const HDdouble TRF[3])
{
    glMatrixMode(GL_PROJECTION); /* Setup perspective projection. */
    glLoadIdentity();

    HDdouble centerScreen[3];
    centerScreen[0] = (TRF[0] + LLB[0])/2.0;
    centerScreen[1] = (TRF[1] + LLB[1])/2.0;
    centerScreen[2] = (TRF[2] + LLB[2])/2.0;

    HDdouble screenDims[3];
    screenDims[0] = TRF[0] - LLB[0];
    screenDims[1] = TRF[1] - LLB[1];
    screenDims[2] = TRF[2] - LLB[2];

    HDdouble maxDimXY = (screenDims[0] > screenDims[1] ? screenDims[0]:screenDims[1]);
    HDdouble maxDim = (maxDimXY > screenDims[2] ? maxDimXY:screenDims[2]);
    maxDim /= 2.0;

    glOrtho(centerScreen[0]-maxDim, centerScreen[0]+maxDim, 
            centerScreen[1]-maxDim, centerScreen[1]+maxDim,
            centerScreen[2]+maxDim, centerScreen[2]-maxDim);
    
    glShadeModel(GL_SMOOTH);

    // Setup model transformations.
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();

    glClearDepth(1.0); 
    glDisable(GL_DEPTH_TEST);
}


/*******************************************************************************    
    Setup graphics pipeline, lights etc.
*******************************************************************************/
void doGraphicsState()
{
    glMatrixMode(GL_MODELVIEW); /* Setup model transformations. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT_MODEL_TWO_SIDE);
    glShadeModel(GL_SMOOTH);
    
    GLfloat lightZeroPosition[] = {10.0, 4.0, 100.0, 0.0};
    GLfloat lightZeroColor[] = {0.6, 0.6, 0.6, 1.0}; /* green-tinted */
    GLfloat lightOnePosition[] = {-1.0, -2.0, -100.0, 0.0};
    GLfloat lightOneColor[] = {0.6, 0.6, 0.6, 1.0}; /* red-tinted */
    
    GLfloat light_ambient[] = {0.8, 0.8, 0.8, 1.0}; /* White diffuse light. */
    GLfloat light_diffuse[] = {0.0, 0.0, 0.0, 1.0}; /* White diffuse light. */
    GLfloat light_position[] = {0.0, 0.0, 100.0, 1.0}; /* Infinite light loc. */
    
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneColor);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

/*******************************************************************************    
 Draw Slave Sphere, at given position.
*******************************************************************************/
void displayVisitorSphere(GLUquadricObj* quadObj, const double position[3])
{
    glMatrixMode(GL_MODELVIEW);        

    glPushMatrix();

    // Display one sphere to represent the haptic cursor and the dynamic 
    // charge.
    glTranslatef(position[0], position[1], position[2]);
    
    // The device-controlled sphere.
    glColor4f(0.9, 0.1, 0.1, 1.0);
    gluSphere(quadObj, VISITOR_SPHERE_RADIUS, 20, 20); 

    glPopMatrix();
}

/*******************************************************************************    
 Draw Master Sphere, at given position
*******************************************************************************/
void displayFixedSphere(GLUquadricObj* quadObj, const double position[3])
{
    // Setup model transformations.
    glMatrixMode(GL_MODELVIEW); 

    glPushMatrix();

    // Display one sphere to represent the haptic cursor and the dynamic 
    // charge.
    glTranslatef(position[0], position[1], position[2]);

    // The center sphere.
    glColor4f(0.2, 0.8, 0.8, 1.0);
    gluSphere(quadObj, FIXED_SPHERE_RADIUS, 20, 20);  

    glPopMatrix();
}

/*****************************************************************************/
