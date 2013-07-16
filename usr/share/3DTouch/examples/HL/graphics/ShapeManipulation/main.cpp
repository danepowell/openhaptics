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

  This example demonstrates dragging 3D objects with constraints.
  It displays a group of objects.  When the user touches one and
  holds the stylus button down, the user can move it around the screen.
  While the button is down, constraint axes are drawn to allow more
  precise control over dragging.

*******************************************************************************/

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
#include <HDU/hduMath.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduQuaternion.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>

#include <vector>
#include <iostream>

/* Function prototypes. */
void glutDisplay();
void glutReshape(int width, int height);
void glutIdle(); 
void glutMouse(int button, int state, int x, int y);
void glutMotion(int x, int y);
void glutKeyboard(unsigned char key, int x, int y);

void initHL();
void initGL();
void initScene();
void drawScene();
void drawString(const char* string);
void drawPrompts();
void updateCamera();
void updateHapticMapping();

void createDraggableObjects();
void drawDraggableObjects();

void HLCALLBACK buttonDownCollisionThreadCallback(HLenum event, HLuint object, 
                                                  HLenum thread, HLcache *cache, 
                                                  void *userdata);
void HLCALLBACK buttonDownClientThreadCallback(HLenum event, HLuint object, 
                                               HLenum thread, HLcache *cache, 
                                               void *userdata);
void HLCALLBACK buttonUpClientThreadCallback(HLenum event, HLuint object, 
                                             HLenum thread, HLcache *cache, 
                                             void *userdata);
void updateDragObjectTransform();


void exitHandler();


static hduVector3Dd gCameraPosWC;
static int gWindowWidth, gWindowHeight;

#define CURSOR_SIZE_PIXELS 20
static double gCursorScale;
static GLint gCursorDisplayList = 0;

/* Variables used by the trackball emulation. */
static hduMatrix gCameraRotation;
static double gCameraScale = 1.0;
static double gCameraTranslationX = 0;
static double gCameraTranslationY = 0;
static bool gIsRotatingCamera = false;
static bool gIsScalingCamera = false;
static bool gIsTranslatingCamera = false;
static int gLastMouseX, gLastMouseY;

static bool gIsProxyManipPoint = false;

static HHD ghHD = HD_INVALID_HANDLE;
static HHLRC ghHLRC = NULL;

static const double kPI = 3.1415926535897932384626433832795;

static HLuint gAxisId;
static hduVector3Dd gAxisCenter(0,0,0);

/* Struct representing one of the shapes in the scene that can be felt, 
   touched and drawn. */
struct DraggableObject
{
    bool bShapeDirty;
    HLuint shapeId;
    GLuint displayList;
    hduMatrix transform;
};

/* List of all draggable objects in scene. */
std::vector<DraggableObject> draggableObjects;

/* Object currently being dragged (index into draggableObjects). */
long int gCurrentDragObj = -1;

/* Position and orientation of proxy at start of drag. */
hduVector3Dd gStartDragProxyPos;
hduQuaternion gStartDragProxyRot;

/* Position and orientation of drag object at start of drag. */
hduMatrix gStartDragObjTransform;

/* Flag for enabling/disabling axis snap on drag. */
bool gAxisSnap = true;

/* flag for enabling/disabling rotation. */
bool gRotate = true;

/*******************************************************************************
 Main function.
*******************************************************************************/
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(500, 500);

    glutCreateWindow("Shape Manipulation Example");

    // Set glut callback functions.
    glutDisplayFunc(glutDisplay);
    glutReshapeFunc(glutReshape);
    glutIdleFunc(glutIdle);
    glutMouseFunc(glutMouse);
    glutMotionFunc(glutMotion);
    glutKeyboardFunc(glutKeyboard);
       
    // The GLUT main loop won't return control, so we need to perform cleanup
    // using an exit handler.
    atexit(exitHandler);

    initScene();    

    glutMainLoop();

    return 0;
}

/*******************************************************************************
 GLUT callback for redrawing the view.  Use this as an opportunity to perform
 graphics rate processing.
*******************************************************************************/
void glutDisplay()
{    
    drawScene();
}

/*******************************************************************************
 GLUT callback for reshaping the window.  This is the main place where the 
 viewing and workspace transforms get initialized.
*******************************************************************************/
void glutReshape(int width, int height)
{
    static const double kFovY = 40;
    static const double kCanonicalSphereRadius = 2;

    glViewport(0, 0, width, height);
    gWindowWidth = width;
    gWindowHeight = height;

    // Compute the viewing parameters based on a fixed fov and viewing
    // sphere enclosing a canonical box centered at the origin.

    double nearDist = kCanonicalSphereRadius / tan((kFovY / 2.0) * kPI / 180.0);
    double farDist = nearDist + 2.0 * kCanonicalSphereRadius;
    double aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);

    // Place the camera down the Z axis looking at the origin.
    gCameraPosWC[0] = 0;
    gCameraPosWC[1] = 0;
    gCameraPosWC[2] = nearDist + kCanonicalSphereRadius;
 
    updateCamera();
}

/*******************************************************************************
 GLUT callback for idle state.  Use this as an opportunity to request a redraw.
*******************************************************************************/
void glutIdle()
{
    glutPostRedisplay();
}

/*******************************************************************************
 Initializes the scene.  Handles initializing both OpenGL and HDAPI. 
*******************************************************************************/
void initScene()
{
    initGL();
    initHL();
    createDraggableObjects();
}

/*******************************************************************************
 Cleanup.
*******************************************************************************/
void exitHandler()
{
    // Free up the haptic rendering context.
    hlMakeCurrent(NULL);
    if (ghHLRC != NULL)
    {
        hlDeleteContext(ghHLRC);
    }

    // Free up the haptic device.
    if (ghHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(ghHD);
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
    
    // Enable depth buffering for hidden surface removal.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    
    // Cull back faces.
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    // Other misc features.
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
 Sets up and initializes haptic rendering library.
*******************************************************************************/
void initHL()
{
    HDErrorInfo error;
    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(1);
    }   
    
    // Create a haptic context for the device.  The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC); 

    // Generate a shape id to hold the axis snap constraint.
    gAxisId = hlGenShapes(1);

    // Add a callback to handle button down in the collision thread.
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY, HL_COLLISION_THREAD, 
                       buttonDownCollisionThreadCallback, NULL);

}

/*******************************************************************************
 Creates the objects that can be seen, felt, and dragged around. 
*******************************************************************************/
void createDraggableObjects()
{
    // Create a bunch of shapes and add them to the draggable object vector.
    DraggableObject dro;
    dro.bShapeDirty = true;
    
    // Sphere.
    dro.shapeId = hlGenShapes(1);
    dro.transform = hduMatrix::createTranslation(0,0,0);
    dro.displayList = glGenLists(1);
    glNewList(dro.displayList, GL_COMPILE);
        glutSolidSphere(0.4, 30, 30);
    glEndList();

    draggableObjects.push_back(dro);

    // Tetrahedron.
    dro.shapeId = hlGenShapes(1);
    dro.transform = hduMatrix::createTranslation(-1.3,0,0);
    dro.displayList = glGenLists(1);
    glNewList(dro.displayList, GL_COMPILE);
        glutSolidTetrahedron();
    glEndList();

    draggableObjects.push_back(dro);

    // Dodecahedron.
    dro.shapeId = hlGenShapes(1);
    dro.transform = hduMatrix::createTranslation(1.4,0,0);
    dro.displayList = glGenLists(1);
    glNewList(dro.displayList, GL_COMPILE);
        glPushMatrix();
        glScalef(0.5,0.5,0.5);
        glutSolidDodecahedron();
        glPopMatrix();
    glEndList();

    draggableObjects.push_back(dro);

    // Cube.
    dro.shapeId = hlGenShapes(1);
    dro.transform = hduMatrix::createTranslation(0,1.4,0);
    dro.displayList = glGenLists(1);
    glNewList(dro.displayList, GL_COMPILE);
        glutSolidCube(0.5);
    glEndList();

    draggableObjects.push_back(dro);

    // Cone.
    dro.shapeId = hlGenShapes(1);
    dro.transform = hduMatrix::createTranslation(0,-1.4,0);
    dro.displayList = glGenLists(1);
    glNewList(dro.displayList, GL_COMPILE);
        glutSolidCone(0.4, 0.6, 30, 30);
    glEndList();

    draggableObjects.push_back(dro);

    // Add event callbacks for button down on each of the shapes.
    // Callbacks will set that shape to be the drag object.
    for (int i = 0; i < draggableObjects.size(); ++i)
    {
        // Pass the index of the object as userdata.
        hlAddEventCallback(HL_EVENT_1BUTTONDOWN, 
                           draggableObjects[i].shapeId, 
                           HL_CLIENT_THREAD, 
                           buttonDownClientThreadCallback, 
                           reinterpret_cast<void *>(i));
    }

    // Add an event callback on button to clear the drag object
    // and end dragging.
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       buttonUpClientThreadCallback, NULL);
}

/*******************************************************************************
 Draws the objects that can be seen, felt and dragged around. 
*******************************************************************************/
void drawDraggableObjects()
{
    hlTouchModel(HL_CONTACT);
    hlTouchableFace(HL_FRONT);

    for (int i = 0; i < draggableObjects.size(); ++i)
    {
        DraggableObject& obj = draggableObjects[i];

        // Position and orient the object.
        glPushMatrix();

        glMultMatrixd(obj.transform);

        // Draw the object graphically.
        glCallList(obj.displayList);

        // Draw the object haptically (but not if it is being dragged).
        if (i != gCurrentDragObj)
        {
            if (obj.bShapeDirty)
            {
                obj.bShapeDirty = false;
                hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, obj.shapeId);
                glCallList(obj.displayList);
                hlEndShape();
            }
            else
            {
                // Once the shape geometry has been captured, it can be referenced
                // on successive frames while still allowing for updates
                // to material attributes, transform, touch model, etc.
                hlCallShape(obj.shapeId);
            }
        }

        glPopMatrix();
    }
}

/*******************************************************************************
 Set the modelview transform from scratch. Apply the current view orientation
 and scale. 
*******************************************************************************/
void updateCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(gCameraPosWC[0], gCameraPosWC[1], gCameraPosWC[2],
              0, 0, 0,
              0, 1, 0);
    
    glTranslatef(gCameraTranslationX, gCameraTranslationY, 0);
    glMultMatrixd(gCameraRotation);
    glScaled(gCameraScale, gCameraScale, gCameraScale);

    updateHapticMapping();

    glutPostRedisplay();
}

/*******************************************************************************
 Use the current OpenGL viewing transforms to initialize a transform for the
 haptic device workspace so that it's properly mapped to world coordinates.
*******************************************************************************/
void updateHapticMapping(void)
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
    gCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    gCursorScale *= CURSOR_SIZE_PIXELS;
}

/******************************************************************************
 Displays a cursor using the current haptic device proxy transform and the
 mapping between the workspace and world coordinates
******************************************************************************/
void redrawCursor()
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

/******************************************************************************
 Draws coordinate axes using OpenGL. 
******************************************************************************/
void drawAxes(const hduVector3Dd& gAxisCenter)
{
    // Lines and points are best viewed without OpenGL lighting.
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);

    // Draw three lines - one along each coordinate axis from -1 to 1.
    glBegin(GL_LINES); 
    {
        glVertex3f(gAxisCenter[0] - 1,gAxisCenter[1],gAxisCenter[2]);
        glVertex3f(gAxisCenter[0] + 1,gAxisCenter[1],gAxisCenter[2]);
        glVertex3f(gAxisCenter[0],gAxisCenter[1] - 1,gAxisCenter[2]);
        glVertex3f(gAxisCenter[0],gAxisCenter[1] + 1,gAxisCenter[2]);
        glVertex3f(gAxisCenter[0],gAxisCenter[1],gAxisCenter[2] - 1);
        glVertex3f(gAxisCenter[0],gAxisCenter[1],gAxisCenter[2] + 1);
    } 
    glEnd();

    // Draw the origin.
    glBegin(GL_POINTS);
    glVertex3f(gAxisCenter[0],gAxisCenter[1],gAxisCenter[2]);
    glEnd();

    glPopAttrib();
}

/******************************************************************************
 The main routine for displaying the scene. 
******************************************************************************/
void drawScene()
{
    
    hlBeginFrame();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Any client thread button callbacks get triggered here.
    hlCheckEvents();

    // Check if button on stylus is down - if so draw
    // the coordinate axes and move the drag object.
    HLboolean buttDown;
    hlGetBooleanv(HL_BUTTON1_STATE, &buttDown);

    if (buttDown)
    {
        if (gAxisSnap)
        {
            // Graphically render the axes.
            drawAxes(gAxisCenter);

            // Make sure proxy resolution is on.  The event handler
            // turns it off but it must be on for shapes to be felt.
            hlEnable(HL_PROXY_RESOLUTION);

            // Haptically render the coordinate axes as a feedback 
            // buffer shape.
            hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gAxisId);

            // Make it a constraint to the cursor will stick to the axes.
            hlTouchModel(HL_CONSTRAINT);

            // Snap distance allows user to pull off of the constraint
            // if the user moves beyond that snap distance.
            hlTouchModelf(HL_SNAP_DISTANCE, 1.5);

            // Call the OpenGL commands to draw the axes but this time
            // they will be used for haptics.
            drawAxes(gAxisCenter);

            hlEndShape();
        }

        // "Drag" the current drag object, if one is current.
        if (gCurrentDragObj != -1)
        {
            updateDragObjectTransform();
        }
    }

    drawDraggableObjects();

    redrawCursor();

    drawPrompts();

    glutSwapBuffers();

    hlEndFrame();
}

/******************************************************************************
 GLUT callback for responding to mouse button presses.  Detect whether to
 initiate a point snapping, view rotation or view scale.
******************************************************************************/
void glutMouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        if (button == GLUT_LEFT_BUTTON)
        {
            gIsRotatingCamera = true;            
        }
        else if (button == GLUT_RIGHT_BUTTON)
        {
            gIsScalingCamera = true;            
        }
        else if (button == GLUT_MIDDLE_BUTTON)
        {
            gIsTranslatingCamera = true;
        }

        gLastMouseX = x;
        gLastMouseY = y;
    }
    else
    {
        gIsRotatingCamera = false;
        gIsScalingCamera = false;
        gIsTranslatingCamera = false;
    }
}

/******************************************************************************* 
 This routine is used by the view rotation code for simulating a virtual
 trackball.  This math computes the z height for a 2D projection onto the
 surface of a 2.5D sphere.  When the input point is near the center of the
 sphere, this routine computes the actual sphere intersection in Z.  When 
 the input point moves towards the outside of the sphere, this routine will 
 solve for a hyperbolic projection, so that it still yields a meaningful answer.
*******************************************************************************/
double projectToTrackball(double radius, double x, double y)
{
    static const double kUnitSphereRadius2D = sqrt(2.0);
    double z;

    double dist = sqrt(x * x + y * y);
    if (dist < radius * kUnitSphereRadius2D / 2.0)
    {
        // Solve for sphere case.
        z = sqrt(radius * radius - dist * dist);
    }
    else
    {
        // Solve for hyperbolic sheet case.
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
    if (gIsTranslatingCamera)
    {
        gCameraTranslationX += 10 * double(x - gLastMouseX)/gWindowWidth;
        gCameraTranslationY -= 10 * double(y - gLastMouseY)/gWindowWidth;

        updateCamera();
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
 GLUT callback for key presses.
*******************************************************************************/
void glutKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':
    case 'R':
        gRotate = !gRotate;
        break;
    case 'a':
    case 'A':
        gAxisSnap = !gAxisSnap;
        break;
    }
}

/******************************************************************************
 Event callback triggered when styus button is depressed.  Called in collision 
 thread to avoid kick.  (If the application instead waits to handle the button 
 down in the client thread, the user can get a kick when the constraint is
 set if the user's hand moved from the position that the button down was
 originally recorded at.)
******************************************************************************/
void HLCALLBACK buttonDownCollisionThreadCallback(HLenum event, HLuint object, 
                                                  HLenum thread, HLcache *cache, 
                                                  void *userdata)
{
    if (gAxisSnap)
    {
        // Use the state cache to get the proxy position at the time
        // the event occured.
        hlCacheGetDoublev(cache, HL_PROXY_POSITION, gAxisCenter);

        // Temporarily turn off proxy resolution and set the
        // proxy position at the proxy position from the time
        // the event occured.  With proxy resolution disabled, the
        // proxy will not move unless told, so it will stick
        // to this spot until we place the constraint in the drawScene
        // routine in the client thread.  This basically allows us to hold
        // the proxy in place until the constraint can hold it there.
        hlDisable(HL_PROXY_RESOLUTION);
        hlProxydv(HL_PROXY_POSITION, gAxisCenter);
    }
}


/******************************************************************************
 Event callback triggered when styus button is depressed and touching one of 
 the draggable objects.  This callback is always called in client thread.
******************************************************************************/
void HLCALLBACK buttonDownClientThreadCallback(HLenum event, HLuint object, 
                                               HLenum thread, HLcache *cache, 
                                               void *userdata)
{
    assert(gCurrentDragObj == -1);
    assert(object != HL_OBJECT_ANY);

    // Clicked while touching an object, set this object to be the current 
    // object being dragged.  When event callback was registered, we set the 
    // index of the drag object as the user data.
    gCurrentDragObj = reinterpret_cast<long int>(userdata);

    // Store off proxy position so we can compute how much it moves each
    // frame (which is how much the drag object should also move).
    hlGetDoublev(HL_PROXY_POSITION, gStartDragProxyPos);
    hlGetDoublev(HL_PROXY_ROTATION, gStartDragProxyRot);

    // Store off initial position and orientation of drag object.
    gStartDragObjTransform = draggableObjects[gCurrentDragObj].transform;
}

/******************************************************************************
 Event callback triggered when styus button is depressed
 and touching one of the draggable objects.
******************************************************************************/
void HLCALLBACK buttonUpClientThreadCallback(HLenum event, HLuint object, 
                                             HLenum thread, HLcache *cache, 
                                             void *userdata)
{
    // Button up, done dragging, clear current drag object.
    gCurrentDragObj = -1;
}

/******************************************************************************
 Calculates updated object transform for drag object based on changes to
 proxy transform.
******************************************************************************/
void updateDragObjectTransform()
{
    assert(gCurrentDragObj >= 0 && 
           gCurrentDragObj < draggableObjects.size());

    // Calculated delta between current proxy pos and proxy pos at start 
    // of drag.
    hduVector3Dd proxyPos;
    hlGetDoublev(HL_PROXY_POSITION, proxyPos);
    hduVector3Dd dragDeltaTransl = proxyPos - gStartDragProxyPos;

    // Same for rotation.
    hduMatrix deltaRotMat;
    if (gRotate)
    {
        hduQuaternion proxyRotq;
        hlGetDoublev(HL_PROXY_ROTATION, proxyRotq);
        hduQuaternion dragDeltaRot = gStartDragProxyRot.inverse() * proxyRotq;
        dragDeltaRot.normalize();
        dragDeltaRot.toRotationMatrix(deltaRotMat);

        // Want to rotate about the proxy position, not the origin,
        // so need to translate to/from proxy pos.
        hduMatrix toProxy = hduMatrix::createTranslation(-gStartDragProxyPos);
        hduMatrix fromProxy = hduMatrix::createTranslation(gStartDragProxyPos);
        deltaRotMat = toProxy * deltaRotMat * fromProxy;
    }

    // Compose rotation and translation deltas.
    hduMatrix deltaMat = deltaRotMat * hduMatrix::createTranslation(dragDeltaTransl);

    // Apply these deltas to the drag object transform.
    draggableObjects[gCurrentDragObj].transform = gStartDragObjTransform * deltaMat;
}

/******************************************************************************
 Draws a string using OpenGL. 
******************************************************************************/
void drawString(const char* string)
{
    for (;*string != '\0';++string)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string);
    }
}

/******************************************************************************
 Draws string prompts at the bottom of the screen.
******************************************************************************/
void drawPrompts()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, gWindowWidth, 0, gWindowHeight, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(4, 54);
    drawString("Touch an object and press button to drag.\n");
    glRasterPos2f(4, 36);
    drawString("Use mouse to rotate, pan, zoom.");
    glRasterPos2f(4, 18);
    drawString("(A) to toggle axis snap, (R) to toggle rotation.");
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

/*****************************************************************************/

