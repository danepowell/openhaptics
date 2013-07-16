/*****************************************************************************
Copyright (c) 2009 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at:
http://dsc.sensable.com

Module Name:
main.cpp

Description:
This example demonstrates the new Depth of Penetration Feature in HLAPI.
This demo also contains special code for loading a 3d OBJ model and haptically
height mapping a texture on top of it. This is a simple haptic displacement
mapping technique.

This example also shows how to integrate HDAPI and HLAPI using hdCallbacks
according to the information given on page 6-38 of OpenHaptics Programmers
Guide.

All models abd textures in this simulation are courtesy of mySmartSimulations, Inc.

Note : This demo has forces and material stiffness specifically designed for the
Phantom Omni and use of a different Phantom may require modifying these parameters
for best performance.

******************************************************************************/

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include <stack>

#if defined(WIN32)
#include <conio.h>
#include <windows.h>
#endif

#if defined(linux)
#include <ncurses.h>
#define _getch getch
#endif

#if defined(WIN32) || defined(linux)
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#endif

#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>
#include <HDU/hduMath.h>
#include <HDU/hduBoundBox.h>

#include <HLU/hlu.h>
#include "GLM.h"
#include "tga.h"

//File Names
char *model1Path("/examples/models/hiResPlane2.obj");
char *model2Path("/examples/models/LumbarBallProbe.obj");
char *texture1Path("/examples/textures/Spine3.tga");

// Tga loader params
tgaInfo *info;
static float bumpScale = 3.0;

/* Mouse Movement */
static int oldMouseX,oldMouseY;
static int currentMouseX,currentMouseY;
static bool isLeftMouseActive;
static bool isRightMouseActive;

float zDepth = -1.5;
float xTranslate = 0.0f;
float yTranslate = 0.0f;
float xRotation  = 0.0f;
float yRotation  = 0.0f;

HLdouble forceScaler = 1.0;

/* Class GLM object*/
GLMmodel* objmodel = NULL;
GLMmodel* toolObjmodel = NULL;

/* Haptic device and rendering context handles. */
static HHD ghHD = HD_INVALID_HANDLE;
static HHLRC ghHLRC = 0;

/* Shape id for shape we will render haptically. */
HLuint gShapeId;
HLuint gPointId;

//Display list for model
GLuint objList;
GLuint bumpList;
GLuint toolObjList;

hduMatrix m_viewTworld;
hduVector3Dd proxyPosition(0.0,0.0,0.0);
double greyScaleData = 0.0;

#define CURSOR_SIZE_PIXELS 20

static double gCursorScale;
static GLuint gCursorDisplayList = 0;
int width, height;

hduVector3Dd proxyNormal(0.0,0.0,0.0);
hduVector3Dd devicePos1(0.0,0.0,0.0);
hduVector3Dd devicePos2(0.0,0.0,0.0);

bool touchedHole = false;
HDdouble probeDop;
hduVector3Dd force(0.0,0.0,0.0);
HLfloat cursorToToolTranslation = 0.25;

/* Function prototypes. */
void glutDisplay(void);
void glutReshape(int width, int height);
void glutIdle(void);
void glutMenu(int);
void glutMouse(int button,int state,int x,int y);
void glutMouseMotion(int x,int y);
void exitHandler(void);
void HandleKeyboard(unsigned char key, int x, int y);

void initGL();
void initOBJModel();
void initToolOBJModel();
void initHL();

void initScene();
void drawSceneHaptics();
void drawSceneGraphics();
void drawCursor();
void updateWorkspace();
double DetermineFPS(void);
void DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...);
int pixelLoadFromImage(char *filename, int normals);
float trilinearInterp(float x, float y, float z);
float getPixel(int x, int y, int z);
void drawGLMbump();
bool buildTGATexture(TextureImage *texture, char *filename);

void HLCALLBACK hlTouchCB(HLenum event, HLuint object,
HLenum thread, HLcache *cache,
void *userdata);

void HLCALLBACK hlUnTouchCB(HLenum event, HLuint object,
HLenum thread, HLcache *cache,
void *userdata);

void DisplayInfo(void);

/*************************************************************
ADDED
*************************************************************/
HDCallbackCode HDCALLBACK hdBeginCB(void *data)
{
// calling hdBeginFrame increments the HD frame counter.
    hdBeginFrame(hdGetCurrentDevice());

    HDErrorInfo error;

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Error in hdBeginCB\n");
        return HD_CALLBACK_DONE;
    }

    return HD_CALLBACK_CONTINUE;
}


HDCallbackCode HDCALLBACK hdEndCB(void *data)

{
    static const HDdouble kRampUpRate = 0.0001;
    static const HDdouble kImpulseLimit = 0.001;

    hdSetDoublev(HD_SOFTWARE_FORCE_IMPULSE_LIMIT,&kImpulseLimit);
    hdSetDoublev(HD_FORCE_RAMPING_RATE, &kRampUpRate );
    hdGetDoublev(HD_CURRENT_FORCE, force);

//Create custom haptic layers. There are two layers here
//at DOP = 0.1  and DOP = 0.35 .

    if (touchedHole && force[2]>=0.0)
    {
        if (probeDop > 0.0 && probeDop < 0.0125)
            force[2] = 0.25;
        else if (probeDop > 0.0 && probeDop < 0.025)
            force[2] = 0.5;
        else if (probeDop > 0.025 && probeDop < 0.05)
            force[2] = 0.75;
        else if (probeDop > 0.05 && probeDop < 0.075)
            force[2] = 1.0;
        else if (probeDop > 0.075 && probeDop < 0.09)
            force[2] = 1.5;
        else if (probeDop > 0.09 && probeDop < 0.1)
            force[2] = 0.0;
        else if (probeDop > 0.1 && probeDop < 0.2)
            force[2] = 0.25;
        else if (probeDop > 0.2 && probeDop < 0.225)
            force[2] = 0.5;
        else if (probeDop > 0.225 && probeDop < 0.25)
            force[2] = 0.75;
        else if (probeDop > 0.25 && probeDop < 0.275)
            force[2] = 0.0;
        else if (probeDop > 0.275 && probeDop < 0.3)
            force[2] = 0.0;
        else if (probeDop > 0.3 && probeDop < 0.35)
            force[2] = 0.25;
        else if (probeDop > 0.35 && probeDop < 0.4)
            force[2] = 0.5;
        else if (probeDop > 0.4)
            force[2] = 0.5;

        hdSetDoublev(HD_CURRENT_FORCE, force*forceScaler);
    }

// calling hdEndFrame decrements the frame counter.
// when the beginFrame counter reaches 0, forces are rendered.
// Note that HL makes calls to hdBeginFrame & hdEndFrame internally

    hdEndFrame(hdGetCurrentDevice());
    HDErrorInfo error;

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Error in hdEndCB\n");
    }

    return HD_CALLBACK_CONTINUE;
}


/*************************************************************
END ADDED
*************************************************************/

/*******************************************************************************
 Initializes GLUT for displaying a simple haptic scene.
*******************************************************************************/

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Haptic OBJ Loader");

// Set glut callback functions.
    glutDisplayFunc(glutDisplay);
    glutReshapeFunc(glutReshape);
    glutIdleFunc(glutIdle);
    glutCreateMenu(glutMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutKeyboardFunc(HandleKeyboard);

#if !defined(linux)
    glutFullScreen();
#endif

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
    drawSceneGraphics();
    drawSceneHaptics();

    static int onceOnly =0;
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
    gluPerspective(45, aspect, 0.1, farDist);

// Place the camera down the Z axis looking at the origin.

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, -1.0+farDist,
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

    char title[40];
    sprintf(title, "Haptic Displacement Mapping %4.1f fps", DetermineFPS());

    glutSetWindowTitle(title);
    glutPostRedisplay();
}


/******************************************************************************
 Popup menu handler.
******************************************************************************/
void glutMenu(int ID)
{
    switch(ID)
    {
        case 0:
            exit(0);
            break;
    }
}

/*******************************************************************************
 Handles GLUT mouse events like button presses
*******************************************************************************/
void glutMouse(int button,int state,int x,int y)
{
    if (state == GLUT_UP)
        switch (button)
        {
            case GLUT_LEFT_BUTTON:
                isLeftMouseActive = false;
                break;

            case GLUT_RIGHT_BUTTON:
                isRightMouseActive = false;
                break;
        }

    if (state == GLUT_DOWN)
    {
        oldMouseX = currentMouseX = x;
        oldMouseY = currentMouseY = y;

        switch (button)
        {
            case GLUT_LEFT_BUTTON:
                isLeftMouseActive = true;
                break;

            case GLUT_RIGHT_BUTTON:
                isRightMouseActive = true;
                break;
        }
    }
}


/*******************************************************************************
Handles GLUT mouse motion event 
*******************************************************************************/
void glutMouseMotion(int x,int y)
{ 
    currentMouseX = x; 
    currentMouseY = y;

    if (isLeftMouseActive && isRightMouseActive) 
    { 
        xTranslate += (currentMouseX - oldMouseX)/100.0f; 
        yTranslate -= (currentMouseY - oldMouseY)/100.0f; 
    } 
    else if (isLeftMouseActive) 
    { 
        xRotation -= (currentMouseX - oldMouseX); 
        yRotation -= (currentMouseY - oldMouseY); 
    } 
    else if (isRightMouseActive) 
        zDepth -= (currentMouseY - oldMouseY)/10.f;

    oldMouseX = currentMouseX;
    oldMouseY = currentMouseY;
}


/*******************************************************************************
 Initializes the scene.  Handles initializing both OpenGL and HL.
*******************************************************************************/
void initScene()
{
//Get the path of models and textures and load them up..

    char descr1[100];
    char *descr = getenv("3DTOUCH_BASE");

    if (descr == NULL)
    { 
        std::cout<<"Warning, 3DTOUCH_BASE not set"<<std::endl;
        descr1[0] = '\0';
    }

    strcpy(descr1,descr) ;

    char descr2[100];
    strcpy(descr2,descr);

    char descr3[100];
    strcpy(descr3,descr);

    model1Path = strcat(descr1, model1Path); 
    model2Path = strcat(descr2, model2Path);
    texture1Path = strcat(descr3, texture1Path);

    std::cout<<"Loading.."<<model1Path<<std::endl;
    std::cout<<"Loading.."<<model2Path<<std::endl;
    std::cout<<"Loading.."<<texture1Path<<std::endl;

//Load the pixels from the image pixel loader...
    int loading = pixelLoadFromImage(texture1Path,0);

    if (!loading)
    {
        fprintf(stderr, "Texture file not found\n");
        _getch();
        exit(0);
    }
    else
    {
        printf("Texture Loading successful \n\n");
    }

    initOBJModel();
    initToolOBJModel();
    initGL();
    initHL();
}


/*******************************************************************************
 Sets up general OpenGL rendering properties: lights, depth buffering, etc.
*******************************************************************************/
void initGL()
{
// Light properties for the diffuse light, specular light, and light position. //
    static const GLfloat light_model_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    static const GLfloat light0_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    static const GLfloat light0_direction[] = {0.0f, -0.4f, 1.0f, 0.0f};
    static const GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};

// Enable depth buffering for hidden surface removal.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

// Cull back faces.
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glClearDepth(1.0f);                                     // Depth Buffer Setup

// Setup other misc features.
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

// Setup lighting model.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
    glEnable(GL_LIGHT0);

// Set up the material information for our objects.
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
}


/*******************************************************************************
 Initialize the OBJ Model and create display list.
*******************************************************************************/
void initOBJModel()
{
    if (!objmodel)
    {
        objmodel = glmReadOBJ(model1Path);

        if (!objmodel)
        {
            printf("OBJ file does not exist \n");
            exit(0);
        }

        glmUnitize(objmodel);
        glmFacetNormals(objmodel);
        glmVertexNormals(objmodel, 90.0);
    }

//Create display for the OBJ model
    objList = glGenLists(1);
    glNewList( objList, GL_COMPILE );
    glmDraw(objmodel, GLM_SMOOTH | GLM_TEXTURE );
    glEndList();

//Create the display list for the modified
// displacement mapped OBJ Model
    bumpList = glGenLists(1);
    glNewList( bumpList, GL_COMPILE );
    drawGLMbump();

    glEndList();
}


/*******************************************************************************
 Initialize the 3d Model for tool .  This involves reading 3ds file, calculating normals,
 calculating tangent space and bounding box.
*******************************************************************************/
void initToolOBJModel()
{
    if (!toolObjmodel)
    {
        toolObjmodel = glmReadOBJ(model2Path);

        if (!toolObjmodel)
        {
            printf("OBJ file does not exist \n");
            exit(0);
        }

        glmUnitize(toolObjmodel);
        glmScale(toolObjmodel, 0.250);
        glmFacetNormals(toolObjmodel);
        glmVertexNormals(toolObjmodel, 90.0);
    }

    toolObjList = glGenLists(1);
    glNewList( toolObjList, GL_COMPILE );
    glmDraw(toolObjmodel, GLM_SMOOTH);
    glEndList();
}


/*******************************************************************************
 Initialize the HDAPI.  This involves initing a device configuration, enabling
 forces, and scheduling a haptic thread callback for servicing the device.
*******************************************************************************/
void initHL()
{
    HDErrorInfo error;
    ghHD = hdInitDevice( HD_DEFAULT_DEVICE );

    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(-1);
    }

/*************************************************************
ADDED
*************************************************************/
    hdScheduleAsynchronous(hdBeginCB, 0, HD_MAX_SCHEDULER_PRIORITY);
    hdScheduleAsynchronous(hdEndCB, 0, HD_MIN_SCHEDULER_PRIORITY);

/*************************************************************
END ADDED
*************************************************************/
    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);

// Enable optimization of the viewing parameters when rendering
// geometry for OpenHaptics.
    hlEnable(HL_HAPTIC_CAMERA_VIEW);

// Generate id for the shape.
    gShapeId = hlGenShapes(1);
    gPointId = hlGenShapes(1);
    hlTouchableFace(HL_FRONT);

    hlAddEventCallback(HL_EVENT_TOUCH,gPointId , HL_COLLISION_THREAD,
        hlTouchCB, 0);

    hlAddEventCallback(HL_EVENT_UNTOUCH,gPointId , HL_COLLISION_THREAD,
        hlUnTouchCB, 0);
}


/*******************************************************************************
 This handler is called when the application is exiting.  Deallocates any state
 and cleans up.
*******************************************************************************/
void exitHandler()
{
// Deallocate the sphere shape id we reserved in initHL.
    if (objmodel)
    {
        hlDeleteShapes(gShapeId, 1);
        hlDeleteShapes(gPointId, 1);
    }

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
 Use the current OpenGL viewing transforms to initialize a transform for the
 haptic device workspace so that it's properly mapped to world coordinates.
*******************************************************************************/
void updateWorkspace()
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

//	HLdouble minn[3]={-0.5,-0.5, 0.5};
//	HLdouble maxx[3]={0.5,0.35,1.5};

    HLdouble minn[3]={-0.4,-0.5, -0.4};
    HLdouble maxx[3]={0.4,0.5,0.4};

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();
    hluFitWorkspaceBox(modelview, minn, maxx);

// Compute cursor scale.
    gCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    gCursorScale *= CURSOR_SIZE_PIXELS;
    hlMatrixMode(HL_MODELVIEW);
    hlLoadMatrixd(modelview);
}


/*******************************************************************************
 The main routine for displaying the scene.  Gets the latest snapshot of state
 from the haptic thread and uses it to display a 3D cursor.
*******************************************************************************/
void drawSceneGraphics()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

// place camera/eye
    glTranslatef(xTranslate ,yTranslate ,zDepth);
    glRotatef(-yRotation, 1.0,0.0,0.0);
    glRotatef(-xRotation, 0.0,1.0,0.0);
    glDisable(GL_TEXTURE_2D);

    drawCursor();

    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glCallList(objList);                      //Displays regular OBJ model

//	glCallList(bumpList); //Displays displacement mapped OBJ model
    glPopMatrix();

//Uncomment to see the Entry Point
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glPointSize(15.0);
    glTranslatef(0.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.05,-0.175,0.975);
    glEnd();

    glPopMatrix();
    glPopAttrib();
    DisplayInfo();
    glEnable(GL_TEXTURE_2D);
    updateWorkspace();
}


/*******************************************************************************
 The main routine for rendering scene haptics.
*******************************************************************************/
void drawSceneHaptics()
{
    hlGetDoublev(HL_PROXY_TOUCH_NORMAL, proxyNormal);
    hlGetDoublev(HL_PROXY_POSITION, proxyPosition);

// Start haptic frame.  (Must do this before rendering any haptic shapes.)
    hlBeginFrame();
    hlPushMatrix();
    hlTouchModel(HL_CONTACT);

// Set material properties for the shapes to be drawn.
    hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.9f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.0f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.1);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION,0.1 );
    hlHinti(HL_SHAPE_FEEDBACK_BUFFER_VERTICES, objmodel->numvertices);
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gShapeId);

// Render haptic shape
    glPushMatrix();
    glCallList(bumpList);
    glPopMatrix();

// End the shape.
    hlEndShape();
    hlPopMatrix();
    hlPushMatrix();
    hlTouchModel(HL_CONSTRAINT);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.4f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.3f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.1);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION,0.1 );

    if (touchedHole && force[2] > -0.1 )
    {
        hlTouchModelf(HL_SNAP_DISTANCE, 300.0);
    }
    else
    {
        hlTouchModelf(HL_SNAP_DISTANCE, 3.0);
    }

    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gPointId);
    glPushMatrix();
    glPointSize(5.0);
    glTranslatef(0.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    glVertex3f(0.05,-0.175,-0.975);
    glEnd();

    glPopMatrix();
    hlEndShape();
    hlPopMatrix();

// End the haptic frame.
    hlEndFrame();
}


/*******************************************************************************
 Draws a 3D cursor for the haptic device using the current local transform,
 the workspace to world transform and the screen coordinate scale.
*******************************************************************************/
void drawCursor()
{
    static const double kCursorRadius = 0.25;
    static const double kCursorHeight = 1.5;
    static const int kCursorTess = 15;

    HLdouble proxyxform[16];
    double proxyPos[3];

    hlGetDoublev(HL_PROXY_POSITION, proxyPos);
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

//If entered hole, then freeze the rotations.
    if (touchedHole)
    {
        proxyxform[0] = 1.0;
        proxyxform[1] = 0.0;
        proxyxform[2] = 0.0;
        proxyxform[3] = 0.0;
        proxyxform[4] = 0.0;
        proxyxform[5] = 1.0;
        proxyxform[6] = 0.0;
        proxyxform[7] = 0.0;
        proxyxform[8] = 0.0;
        proxyxform[9] = 0.0;
        proxyxform[10] = 1.0;
        proxyxform[11] = 0.0;
    }

//Get the depth of Penetration from HLAPI.
    hlGetDoublev(HL_DEPTH_OF_PENETRATION, &probeDop);
    glMultMatrixd(proxyxform);

    if (touchedHole)
        cursorToToolTranslation = 0.35;
    else
        cursorToToolTranslation = 0.25;

    glTranslatef(0.0,0.0,cursorToToolTranslation);

    if (touchedHole && force[2]>=0.0)
        glTranslatef(0.0,0.0,-1* probeDop);

    glCallList(toolObjList);
    glPopMatrix();
    glPopAttrib();
}


/*******************************************************************************
 Determines the frames per second at which the demo is running
*******************************************************************************/
double DetermineFPS(void)
{
    static unsigned int lastMs = glutGet(GLUT_ELAPSED_TIME);
    unsigned int        currentMs = glutGet(GLUT_ELAPSED_TIME);
    unsigned int        framesMs = currentMs - lastMs;
    static unsigned int framesCounted = 0;
    static double fps = 0;

    framesCounted++;

#define DURATION_TO_TIME        1000                    // duration in miliseconds for calculating fps

    if(framesMs >= DURATION_TO_TIME)
    {
        fps = (double)1000 * framesCounted / framesMs;

//printf("last: %ld  curr: %ld\n", g.lastClock, currentClock);
//printf("clocks: %d  cps: %d  FPS: %f\n", clock(), CLOCKS_PER_SEC, fps);

        framesCounted = 0;

//g.framesTime = 0;
        lastMs = currentMs;
    }

    return fps;
}


/*******************************************************************************
    Function:	DrawBitmapString
    Usage:		DrawBitmapString(x, y, GLUT_BITMAP_8_BY_13, "Test")

---------------------------------------------------------------------------
Draws a string using a bitmap font.  See GLUT documentation for the names
of fonts.

*******************************************************************************/
void DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...)
{
    int len, i;
    va_list args;
    char string[256];

// special C stuff to interpret a dynamic set of arguments specified by "..."
    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);

    glRasterPos2f(x, y);
    len = (int) strlen(string);

    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}


/*******************************************************************************
    Function:	HandleKeyboard
    Usage:		HandleKeyboard(key, x, y)

---------------------------------------------------------------------------
Handles some useful keyboard shortcuts.

*******************************************************************************/
void HandleKeyboard(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);
}


/*******************************************************************************
    Function:	textureLoadFromImage
    Usage:		extureLoadFromImage(char *filename, int normals)

---------------------------------------------------------------------------
Extracts the image data pixels

*******************************************************************************/
int pixelLoadFromImage(char *filename, int normals)
{
    int mode;
    float pointHeight;

// load the image, using the tgalib
    info = tgaLoad(filename);

// check to see if the image was properly loaded
// remember: only greyscale, RGB or RGBA noncompressed images
    mode = 1;

    if (info->status != TGA_OK)
    {
        printf("Error loading texture map \n");
        mode = 0;
    }

    return(mode);
}


/*******************************************************************************
 This is a simple trilinear interpolation function. Here the texture coordinates
 of the model are interpolated and mapped to the right pixel values from the image
data loaded before.
*******************************************************************************/
float trilinearInterp(float x, float y, float z)
{
    float px = x * info->width  - 0.5f;
    float py = y * info->height - 0.5f;
    float pz = z * info->pixelDepth - 0.5f;

    if( px < 0 ) px = 0;
    if( py < 0 ) py = 0;
    if( pz < 0 ) pz = 0;

    float fx = floor( px );
    float fy = floor( py );
    float fz = floor( pz );
    float cx = ceil( px );
    float cy = ceil( py );
    float cz = ceil( pz );

    if( cx >= info->width ) cx--;
    if( cy >= info->height) cy--;
    if( cz >= info->pixelDepth ) cz--;

    float xd = px - fx;
    float yd = py - fy;
    float zd = pz - fz; 
    float fff = getPixel( (int)fx, (int)fy, (int)fz ); 
    float ffc = getPixel( (int)fx, (int)fy, (int)cz ); 
    float fcf = getPixel( (int)fx, (int)cy, (int)fz ); 
    float fcc = getPixel( (int)fx, (int)cy, (int)cz ); 
    float cff = getPixel( (int)cx, (int)fy, (int)fz );
    float cfc = getPixel( (int)cx, (int)fy, (int)cz );
    float ccf = getPixel( (int)cx, (int)cy, (int)fz );
    float ccc = getPixel( (int)cx, (int)cy, (int)cz );
    float i1 = fff * (1-zd) + ffc * zd;
    float i2 = fcf * (1-zd) + fcc * zd;
    float j1 = cff * (1-zd) + cfc * zd;
    float j2 = ccf * (1-zd) + ccc * zd;
    float w1 = i1 * (1-yd) + i2 * yd;
    float w2 = j1 * (1-yd) + j2 * yd;
    float v = w1 * (1-xd) + w2 * xd;

    return v;
}


/******************************************************************************
  Get the correct pixel value from the image pixel data loaded before
 ******************************************************************************/
float getPixel(int x, int y, int z)
{
    unsigned int byte_rem = info->pixelDepth % 8;
    unsigned int bytes_per_pixel = info->pixelDepth / 8;
    float magicNum;

    int pixelValue = ( ( z * info->height + y ) * info->width + x ) * bytes_per_pixel ;

    magicNum = info->imageData[pixelValue ];
    return magicNum;
}


/*******************************************************************************
  The obj model is modified by extruding the surface along the direction of
  normals based on the height map (bumpValue) generated by the texture image.
*******************************************************************************/
void drawGLMbump()
{
    static GLMtriangle* trianglE;
    static float bumpValue1,bumpValue2, bumpValue3;
    hduVector3Dd verts;

    glBegin(GL_TRIANGLES);

    for (int i = 0; i < objmodel->numtriangles; i++)
    {
        trianglE = (&objmodel->triangles[i]);
        glNormal3fv(&objmodel->normals[3 * trianglE->nindices[0]]);

        bumpValue1 = trilinearInterp(objmodel->texcoords[2 * trianglE->tindices[0]+0], objmodel->texcoords[2 * trianglE->tindices[0]+1], 0.0);

        bumpValue1 = bumpValue1/(info->width * bumpScale);

        glTexCoord2fv(&objmodel->texcoords[2 * trianglE->tindices[0]]);

        verts[0] = objmodel->vertices[3 * trianglE->vindices[0]+0] + bumpValue1 * objmodel->normals[3 * trianglE->nindices[0] + 0];

        verts[1] = objmodel->vertices[3 * trianglE->vindices[0]+1] + bumpValue1 * objmodel->normals[3 * trianglE->nindices[0] + 1];

        verts[2] = objmodel->vertices[3 * trianglE->vindices[0]+2] + bumpValue1 * objmodel->normals[3 * trianglE->nindices[0] + 2];

        glVertex3f(verts[0], verts[1], verts[2]);
        glNormal3fv(&objmodel->normals[3 * trianglE->nindices[1]]);

        bumpValue2 = trilinearInterp(objmodel->texcoords[2 * trianglE->tindices[1]+0], objmodel->texcoords[2 * trianglE->tindices[1]+1], 0.0);

        bumpValue2 = bumpValue2/(info->width * bumpScale);

        glTexCoord2fv(&objmodel->texcoords[2 * trianglE->tindices[1]]);

        verts[0] = objmodel->vertices[3 * trianglE->vindices[1]+0] + bumpValue2 * objmodel->normals[3 * trianglE->nindices[1] + 0];

        verts[1] = objmodel->vertices[3 * trianglE->vindices[1]+1] + bumpValue2 * objmodel->normals[3 * trianglE->nindices[1] + 1];

        verts[2] = objmodel->vertices[3 * trianglE->vindices[1]+2] + bumpValue2 * objmodel->normals[3 * trianglE->nindices[1] + 2];

        glVertex3f(verts[0], verts[1], verts[2]);
        glNormal3fv(&objmodel->normals[3 * trianglE->nindices[2]]);

        bumpValue3 = trilinearInterp(objmodel->texcoords[2 * trianglE->tindices[2]+0], objmodel->texcoords[2 * trianglE->tindices[2]+1], 0.0);

        bumpValue3 = bumpValue3 /(info->width * bumpScale);

        glTexCoord2fv(&objmodel->texcoords[2 * trianglE->tindices[2]]);

        verts[0] = objmodel->vertices[3 * trianglE->vindices[2]+0] + bumpValue3 * objmodel->normals[3 * trianglE->nindices[2] + 0];

        verts[1] = objmodel->vertices[3 * trianglE->vindices[2]+1] + bumpValue3 * objmodel->normals[3 * trianglE->nindices[2] + 1];

        verts[2] = objmodel->vertices[3 * trianglE->vindices[2]+2] + bumpValue3 * objmodel->normals[3 * trianglE->nindices[2] + 2];

        glVertex3f(verts[0], verts[1], verts[2]);
    }

    glEnd();
}


/*****************************************************************************
    Basic Touch callback. Not used in the demo. Left for user customization.
 *****************************************************************************/
void HLCALLBACK hlTouchCB(HLenum event, HLuint object,
    HLenum thread, HLcache *cache, void *userdata)
{
    touchedHole = true;

    printf("Entered hole in spine\n");
}


void HLCALLBACK hlUnTouchCB(HLenum event, HLuint object,
    HLenum thread, HLcache *cache, void *userdata)
{
    if (force[2] < -0.1)
        touchedHole = false;

    printf("Out of hole in spine\n");
}


/*****************************************************************************
   Function to display text info
 *****************************************************************************/
void DisplayInfo(void)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glLoadIdentity();

    int gwidth, gheight;
    gwidth = glutGet(GLUT_WINDOW_WIDTH);
    gheight = glutGet(GLUT_WINDOW_HEIGHT);

// switch to 2d orthographic mMode for drawing text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, gwidth, gheight, 0);
    glMatrixMode(GL_MODELVIEW);

    glColor3f(1.0, 1.0, 1.0);

    int textRowDown = 0;                                    // lines of text already drawn downwards from the top

    int textRowUp = 0;                                      // lines of text already drawn upwards from the bottom

    DrawBitmapString(0 , 80 , GLUT_BITMAP_HELVETICA_18, "INSTRUCTIONS: ");
    DrawBitmapString(0 , 100 , GLUT_BITMAP_HELVETICA_18, "Use Right-click menu to Quit the Demo");

    DrawBitmapString(0 , gheight-100 , GLUT_BITMAP_HELVETICA_18, "LAYER 1 penetrated: ");

    if (probeDop > 0.1 && touchedHole)
        DrawBitmapString(200 , gheight-100 , GLUT_BITMAP_HELVETICA_18, "Yes");
    else
        DrawBitmapString(200 , gheight-100 , GLUT_BITMAP_HELVETICA_18, "No");

    DrawBitmapString(0 , gheight-80 , GLUT_BITMAP_HELVETICA_18, "LAYER 2 penetrated: ");

    if (probeDop > 0.25 && touchedHole)
        DrawBitmapString(200 , gheight-80 , GLUT_BITMAP_HELVETICA_18, "Yes");
    else
        DrawBitmapString(200 , gheight-80 , GLUT_BITMAP_HELVETICA_18, "No");

    DrawBitmapString(0 , gheight-60 , GLUT_BITMAP_HELVETICA_18, "Depth of Penetration: %f ",probeDop );

    DrawBitmapString(gwidth/2, gheight - 300 , GLUT_BITMAP_TIMES_ROMAN_24, "Entry Point");

    DrawBitmapString(gwidth-250, gheight - 80 , GLUT_BITMAP_HELVETICA_12, "Models & Textures courtesy ");

    DrawBitmapString(gwidth-250, gheight - 60 , GLUT_BITMAP_HELVETICA_18, "mySmartSimulations, Inc.");

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

// turn depth and lighting back on for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

/******************************************************************************/
