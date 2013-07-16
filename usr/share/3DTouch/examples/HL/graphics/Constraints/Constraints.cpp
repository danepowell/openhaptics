/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  Constraints.cpp

Description: 

  This example demonstrates haptic rendering of constraints
  (shapes that stick the device to themselvse) using the HL_CONSTRAINT 
  touch model.

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

static HHD hHD = HD_INVALID_HANDLE;
static HHLRC hHLRC = 0;

/* Shape ids for haptically rendered shapes. */
HLuint gSurfaceShapeId = 0;
HLuint gLineShapeId = 0;
HLuint gHapticsStringShapeId = 0;
HLuint gPlaneShapeId = 0;

#define CURSOR_SIZE_PIXELS 20
static double gCursorScale = 0;
static GLuint gCursorDisplayList = 0;

static GLfloat surfacePos[] = {0, -0.6, 0};
static GLfloat linePos[] = {-0.4, 0, 0};
static GLfloat planePos[] = {-0.8, 0.5,0};

/* Shape snap distances. */
static double gHapticsStringShapeSnapDistance = 0.0;
static double gLineShapeSnapDistance = 0.0;
static double gSurfaceShapeSnapDistance = 0.0;

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

/*******************************************************************************
 Initializes GLUT for displaying a simple haptic scene.
*******************************************************************************/
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);
    glutCreateWindow("Constraints Example");

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

     //Make sure width & height dont go to zero when 
    //window is minimized..
    if(width || height)
    {
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
}

/*******************************************************************************
 GLUT callback for idle state.  Use this to request a redraw.
*******************************************************************************/
void glutIdle()
{
    glutPostRedisplay();
}

/******************************************************************************
 Popup menu handler.
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
    gSurfaceShapeId = hlGenShapes(1);
    gLineShapeId = hlGenShapes(1);
    gHapticsStringShapeId = hlGenShapes(1);
    gPlaneShapeId = hlGenShapes(1);

    // Compute the snap distances to use based on approximating how much
    // force the user should need to exert to pull off (in Newtons).
    const double kHapticsStringShapeForce = 2.0;
    const double kLineShapeForce = 7.0;
    const double kSurfaceShapeForce = 4.0;

    HDdouble kStiffness;
    hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &kStiffness);

    // We can get a good approximation of the snap distance to use by
    // solving the following simple force formula: 
    // >  F = k * x  <
    // F: Force in Newtons (N).
    // k: Stiffness control coefficient (N/mm).
    // x: Displacement (i.e. snap distance).
    gHapticsStringShapeSnapDistance = kHapticsStringShapeForce / kStiffness;
    gLineShapeSnapDistance = kLineShapeForce / kStiffness;
    gSurfaceShapeSnapDistance = kSurfaceShapeForce / kStiffness;
}

/*******************************************************************************
 This handler is called when the application is exiting.  Deallocates any state 
 and cleans up.
*******************************************************************************/
void exitHandler()
{
    // Deallocate the shape ids that were reserved in initHL.
    hlDeleteShapes(gSurfaceShapeId, 1);
    hlDeleteShapes(gLineShapeId, 1);
    hlDeleteShapes(gHapticsStringShapeId, 1);
    hlDeleteShapes(gPlaneShapeId, 1);

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
 Uses the current OpenGL viewing transforms to initialize a transform for the
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

void drawString(const char* string)
{
    for (;*string != '\0';++string)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *string);
    }
}

void drawPlane()
{
    static GLuint displayList = 0;

    if (displayList)
    {
        glCallList(displayList);
    }
    else
    {
        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE_AND_EXECUTE);
        glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
        const GLfloat mat[] = {0, 0, 1};
        glEnable(GL_LIGHTING);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
        glPolygonOffset(1,1);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glBegin(GL_QUADS);
        glNormal3f(0,0,1);
        glVertex3f(-0.1, -0.15, 0);
        glVertex3f(1.4, -0.15, 0);
        glVertex3f(1.4, 0.35, 0);
        glVertex3f(-0.1, 0.35, 0);
        glEnd();
        glDisable(GL_POLYGON_OFFSET_FILL);
        glPopAttrib();
        glEndList();
    }
}

void drawHapticsString()
{
    static GLuint displayList = 0;

    if (displayList)
    {
        glCallList(displayList);
    }
    else
    {
        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE_AND_EXECUTE);
        glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
        glColor3f(1.0,0,0);
        glLineWidth(4.0);
        glPushMatrix();
        glScalef(0.003,0.003,0.003);
        drawString("Haptics");
        glPopMatrix();
        glPopAttrib();
        glEndList();
    }
}

void drawLine()
{
    static GLuint displayList = 0;

    if (displayList)
    {
        glCallList(displayList);
    }
    else
    {
        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE_AND_EXECUTE);
        glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
        glLineWidth(2.0);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(-0.8, -0.8, -0.8);
        glEnd();
        glPointSize(4);
        glBegin(GL_POINTS);
        glVertex3f(0, 0, 0);
        glVertex3f(-0.4, -0.4, -0.4);
        glVertex3f(-0.8, -0.8, -0.8);
        glEnd();
        glPopAttrib();
        glEndList();
    }
}

//Note: We are not using display lists(see below) for the NURB surface because of a known
//Mesa bug: Feb 14, 2001, Mesa 3.4.1 : NURBS or evaluator surfaces inside
//display lists don't always work. This bug mainly maifests under compilation
//on gcc or g++.

void drawSurface()
{
        glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);

        glEnable(GL_LIGHTING);
        glEnable(GL_AUTO_NORMAL);
        glEnable(GL_NORMALIZE);
        GLfloat diffuse[] = { 0.3, 0.3, 0.5, 1 };
        GLfloat specular[] = { 0.7, 0.7, 0.7, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0);

        const int nPointsU = 16;
        const int nPointsV = 4;
        const int order = 4;
        const int nKnotsU = nPointsU + order;
        const int nKnotsV = nPointsV + order;

        // Knots.
        GLfloat uknots[nKnotsU];
        GLfloat vknots[nKnotsV];

        // Allocate uniform knots between 0 and 1 in u and v, with triple 
        // knots at start and end of curve.
        int i, j;
        for (i = 0; i < 3; ++i)
        {
            uknots[i] = vknots[i] = 0;
            uknots[nKnotsU - i- 1] = vknots[nKnotsV - i- 1] = 1;
        }

        for (i = 3; i < nKnotsU - 3; ++i)
        {
            uknots[i] = float(i-3)/float(nKnotsU - 7);
        }

        for (i = 3; i < nKnotsV - 3; ++i)
        {
            vknots[i] = float(i-3)/float(nKnotsV - 7);
        }


        const float uwidth = 1.2;
        const float vwidth = 0.8;
        const float amplitude = 0.3;

        // Control points: Uniform grid with z coordinate varying based 
        // on sine wave.
        typedef GLfloat ControlPoint[3];
        ControlPoint controlPoints[nPointsU][nPointsV];

        for (i = 0; i < nPointsU; ++i)
        {
            for (j = 0; j < nPointsV; ++j)
            {
                controlPoints[i][j][0] = (float)uwidth * (float)i/float(nPointsU);
                controlPoints[i][j][1] = (float)vwidth * (float)j/float(nPointsV);
                controlPoints[i][j][2] = amplitude * sin(2 * 3.14159 * (float)i/nPointsU);
            }
        }

        GLUnurbsObj *surf = gluNewNurbsRenderer();
            gluBeginSurface(surf);
        gluNurbsSurface(surf,nKnotsU, uknots, nKnotsV, vknots,
                        3 * nPointsV, 3, &controlPoints[0][0][0],
                         order, order, GL_MAP2_VERTEX_3);
        gluEndSurface(surf);
        glPopAttrib();
}

/*******************************************************************************
 The main routine for displaying the scene.  Gets the latest snapshot of state
 from the haptic thread and uses it for displaying a 3D cursor.
*******************************************************************************/
void drawSceneGraphics()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    
    // Draw 3D cursor at haptic device position.
    drawCursor();

    glPushMatrix();
    glTranslatef(planePos[0], planePos[1], planePos[2]);
    drawHapticsString();
    drawPlane();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(linePos[0], linePos[1], linePos[2]);
    drawLine();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(surfacePos[0], surfacePos[1], surfacePos[2]);
    drawSurface();
    glPopMatrix();
}

/*******************************************************************************
 The main routine for rendering scene haptics.  Renders the sphere haptically.
*******************************************************************************/
void drawSceneHaptics()
{    
    // Start haptic frame.  Must do this before rendering any haptic shapes.
    hlBeginFrame();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        

    // Use OpenGL commands to create geometry.
    glPushMatrix();
    glTranslatef(planePos[0], planePos[1], planePos[2]);

    // String shape.
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gHapticsStringShapeId);
    hlTouchModel(HL_CONSTRAINT);
    hlTouchModelf(HL_SNAP_DISTANCE, gHapticsStringShapeSnapDistance);
    hlMaterialf(HL_FRONT, HL_STIFFNESS, 0.5);
    hlMaterialf(HL_FRONT, HL_STATIC_FRICTION, 0);
    hlMaterialf(HL_FRONT, HL_DYNAMIC_FRICTION, 0);
    drawHapticsString();
    hlEndShape();

    // Plane shape.
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gPlaneShapeId);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.8);
    hlMaterialf(HL_FRONT, HL_STATIC_FRICTION, 0);
    hlMaterialf(HL_FRONT, HL_DYNAMIC_FRICTION, 0);
    hlTouchModel(HL_CONTACT);
    drawPlane();
    hlEndShape();

    glPopMatrix();

    // Line shape.
    glPushMatrix();
    glTranslatef(linePos[0], linePos[1], linePos[2]);
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gLineShapeId);
    hlTouchModel(HL_CONSTRAINT);
    hlTouchModelf(HL_SNAP_DISTANCE, gLineShapeSnapDistance);
    hlMaterialf(HL_FRONT, HL_STIFFNESS, 0.2);
    hlMaterialf(HL_FRONT, HL_STATIC_FRICTION, 0);
    hlMaterialf(HL_FRONT, HL_DYNAMIC_FRICTION, 0);
    drawLine();
    hlEndShape();
    glPopMatrix();

    // Surface shape.
    glPushMatrix();
    glTranslatef(surfacePos[0], surfacePos[1], surfacePos[2]);
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gSurfaceShapeId);
    hlTouchModel(HL_CONSTRAINT);
    hlTouchModelf(HL_SNAP_DISTANCE, gSurfaceShapeSnapDistance);
    hlMaterialf(HL_FRONT, HL_STIFFNESS, 0.8);
    hlMaterialf(HL_FRONT, HL_STATIC_FRICTION, 0.3);
    hlMaterialf(HL_FRONT, HL_DYNAMIC_FRICTION, 0.5);
    drawSurface();
    hlEndShape();
    glPopMatrix();

    // End the haptic frame.
    hlEndFrame();
}


/*******************************************************************************
 Draws a 3D cursor for the haptic device using the current local transform,
 the workspace to world transform and the screen coordinate scale.
 *******************************************************************************/
void drawCursor()
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
    
    // Get the proxy transform in world coordinates.
    hlGetDoublev(HL_PROXY_TRANSFORM, proxyxform);
    glMultMatrixd(proxyxform);

    // Apply the local cursor scale factor.
    glScaled(gCursorScale, gCursorScale, gCursorScale);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(gCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}

/******************************************************************************/
