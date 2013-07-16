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

  Demo application of simple deformable surface.
  
  Surface is implemented using a matrix of damped springs and masses.

This source code example may be protected by the following U.S. Patents:  6,191,796,
and may be protected by other U.S. patents or foreign patents. Other patents pending.

*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iomanip>

#if defined(WIN32) || defined(linux)
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#endif

#include <HL/hl.h>
#include <HDU/hdu.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>

#include "draw_string.h"
#include "DynamicsMath.h"
#include "ParticleSystem.h"
#include "Constraint.h"
#include "SpringConstraint.h"
#include "MouseSpringConstraint.h"
#include "Surface.h"
#include "UnProjectUtilities.h"

const int kMaxStepSizeMs = 33;

// default window dimensions
const int kWindowWidthDefault = 640;
const int kWindowHeightDefault = 480;

GLint	mWindW=kWindowWidthDefault,
		mWindH=kWindowHeightDefault;

int		mMouseDownX = 0;
int		mMouseDownY = 0;	// mouse down coord

bool	mUseFixedTimeStep = false;
double	mManualTimeStep = 0.033;	// time to increment by when stepping manually
double mTimeStep = 0;	// size of last time step (in seconds)
bool	mPause = false;	// pause the simulation

bool	mDesign = true;	// in design mode or simulation mode

enum EDesignMode
{
	designMode_Particle,
	designMode_Nail,
	designMode_Spring
};

EDesignMode		mMode = designMode_Particle;	// determines what clicking the mouse will add

Surface mSurface;
const int kSurfaceParticlesDef = 10;
const double kSurfaceSize = 10;
int mSurfaceParticles = kSurfaceParticlesDef; // num particles in x and z direction making mSurface grid

HLuint mSurfaceShapeId;
HHD hHD = HD_INVALID_HANDLE;
HHLRC hHLRC = NULL;
double mCursorScale;
GLuint mCursorDisplayList;

enum EDrawInfo {
	drawInfo_StartOfList,
	drawInfo_All = drawInfo_StartOfList,
	drawInfo_FPS,
	drawInfo_None,
	drawInfo_PastEndOfList // enum value for easy cyclying; not a valid option
};

EDrawInfo mDrawInfo = drawInfo_All;
bool mDrawNormals = false;
bool mDrawSurface = true;
bool mDrawSystem = false;

ParticleSystem mPS;	// the particle system

// keep track of constraint while adding it
SpringConstraint* mCurrSpringConstraint = NULL;

std::string mSurfaceConditionsName;

// prototypes
void CreateMenus(void);
void InitHL(void);
void HandleMenuCommand(int option);
void Reshape(int width, int height);
void Display(void);
void RedrawCursor(void);
void UpdateHapticMapping(void);
void Idle(void);
void AdvanceTime(double t);
double GetTime(void);

void HandleMouseButton(int button, int state, int x, int y);
void HandleMouseMotion(int x, int y);
void HandleKeyboard(unsigned char key, int x, int y);
void HandleSpecialKeys(int key, int x, int y);

void HLCALLBACK OnDragSurface(HLenum event, HLuint object, 
                              HLenum thread, HLcache *cache, 
                              void *userdata);
void HLCALLBACK OnTouchSurface(HLenum event, HLuint object, 
                              HLenum thread, HLcache *cache, 
                              void *userdata);

void CreateMenus(void);
void InitDisplay(void);
void ConstructSurface(const int mSurfaceParticles);

void DoProjectionTransformation(void);
void DoViewingTransformation(void);
void DisplayInfo(void);
double DetermineFPS(void);
void exitHandler();

void materialsTest(void);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	
	InitDisplay();
	
    // setup haptic renderer
    InitHL();

	HandleMenuCommand('d'); // reset the surface properties to default

	//mPS.StartConstructingSystem();
	//mDesign = true;
	ConstructSurface(mSurfaceParticles);
	mPause = false;

	// create pulldown menus
	CreateMenus();

    /* The GLUT main loop won't return control, so we need to perform cleanup
       using an exit handler. */
    atexit(exitHandler);

    // turn over control to GLUT
	glutMainLoop();

	return 0;             
}

void ConstructSurface(const int mSurfaceParticles)
{
	if (!mDesign)
	{
		mDesign = true;
	}
	
	mSurface.SetParticleSystem(&mPS);
	mSurface.ConstructSurface(mSurfaceParticles, kSurfaceSize);

	mDesign = false;
}

void InitDisplay(void)
{

	/* display modes: 24 BIT, double buffer mMode */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowSize(mWindW,mWindH);
	glutCreateWindow("Simple Deformable Surface");

	/* clear the display */
	glClear(GL_COLOR_BUFFER_BIT);

	/* set default attributes */
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glLineWidth(1);
	glPointSize(4);


	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat matShininess[] = { 50.0 };
	GLfloat lightPosition[] = { -5.0, 10.0, -5.0, 0.0 };
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
	
	/*  window redisplay and reshape event callbacks */
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape); 

	/* setup mouse handler callbacks */
	//glutPassiveMotionFunc(HandleMouseMotion);
	glutMotionFunc(HandleMouseMotion);
	glutMouseFunc(HandleMouseButton);
	
	glutKeyboardFunc(HandleKeyboard);
	glutSpecialFunc(HandleSpecialKeys);
	
	glutIdleFunc(Idle);
}

/*
 	Function:	InitHL
 	Usage:		InitHL();
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
    
    // Create a haptic context for the device. The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC); 

    // generate a shape id to hold the haptic surface rep
    mSurfaceShapeId = hlGenShapes(1);

    // add a callback to handle motion while touching surface
    hlAddEventCallback(HL_EVENT_TOUCH, mSurfaceShapeId, HL_CLIENT_THREAD, 
                       OnTouchSurface, NULL);
    hlAddEventCallback(HL_EVENT_UNTOUCH, mSurfaceShapeId, HL_CLIENT_THREAD, 
                       OnTouchSurface, NULL);
    // set motion tolerance to 0 so we get motion event when
    // when not moving.
    hlEventd(HL_EVENT_MOTION_LINEAR_TOLERANCE, 0);
    hlAddEventCallback(HL_EVENT_MOTION, mSurfaceShapeId, HL_CLIENT_THREAD, 
                       OnDragSurface, NULL);

}

/*******************************************************************************
 Cleanup
*******************************************************************************/
void exitHandler()
{
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

/*
 	Function:	CreateMenus
 	Usage:		CreateMenus();
 	---------------------------------------------------------------------------
 	Creates the menu which popups when the right mouse is clicked.
  		
 */
void CreateMenus(void)
{
/*	// Uncomment for interactive design of the system
	int modeMenu;
	modeMenu = glutCreateMenu(HandleMenuCommand);
	glutAddMenuEntry("Particle", 'mp');
	glutAddMenuEntry("Nail", 'mn');
	glutAddMenuEntry("Spring", 'ms');
*/

	glutCreateMenu(HandleMenuCommand);
	
	glutAddMenuEntry("Pause (space)", ' ');
	glutAddMenuEntry("Manual Advance (m)", 'm');
	glutAddMenuEntry("Use Fixed Time Step (x)", 'x');
	glutAddMenuEntry("-", 0);

/*	// Uncomment for interactive design of the system
	glutAddMenuEntry("Design / Simulate", 'de');
	glutAddSubMenu("Adding", modeMenu);
	glutAddMenuEntry("Clear System", 'cs');
	glutAddMenuEntry("Construct Test System", 'te');
	glutAddMenuEntry("-", 0);
*/

	glutAddMenuEntry("Default (d)", 'd');
	glutAddMenuEntry("Trampoline (t)", 't');
	glutAddMenuEntry("Foam (f)", 'f');
	glutAddMenuEntry("Jello (j)", 'j');
	glutAddMenuEntry("Waterbed (w)", 'w');
	glutAddMenuEntry("Clay (c)", 'c');
	glutAddMenuEntry("-", 0);

	glutAddMenuEntry("Surface Resolution: 3", '1');
	glutAddMenuEntry("Surface Resolution: 5", '2');
	glutAddMenuEntry("Surface Resolution: 10", '3');
	glutAddMenuEntry("Surface Resolution: 15", '4');
	glutAddMenuEntry("Surface Resolution: 20", '5');
	glutAddMenuEntry("Surface Resolution: 25", '6');
	glutAddMenuEntry("-", 0);

	glutAddMenuEntry("Toggle Show Information (i)", 'i');
	glutAddMenuEntry("Toggle Draw Surface (s)", 's');
	glutAddMenuEntry("Toggle Draw Normals (n)", 'n');
	glutAddMenuEntry("Toggle Draw Particle-Spring System (p)", 'p');
	glutAddMenuEntry("-", 0);

	glutAddMenuEntry("Quit", 'q');
	
#ifdef macintosh
	glutAttachMenuName(GLUT_RIGHT_BUTTON, "Options");
#else
	glutAttachMenu(GLUT_RIGHT_BUTTON);
#endif
}

// Handle menu commands
void HandleMenuCommand(int option)
{
	switch(option)
	{
		case 'de':
			mDesign = !mDesign;
			
			if (mDesign)
			{
				mPS.StartConstructingSystem();
			}
			else
			{
				mPS.FinishConstructingSystem();
			}
			break;
		
		case 'cs':
			if (!mDesign)
			{
				mDesign = true;
				mPS.StartConstructingSystem();
			}
			mPS.ClearSystem();
			break;
		
		case 'mp':
			mMode = designMode_Particle;
			break;

		case 'mn':
			mMode = designMode_Nail;
			break;

		case 'ms':
			mMode = designMode_Spring;
			break;
		
		case 'd':
			mSurfaceConditionsName = "Default";
			mSurface.SetAmbientColor( 0.0, 0.0, 1.0, 1.0 );
			mSurface.SetDiffuseColor( 0.0, 0.0, 1.0, 1.0 );
			mSurface.SetSpecularColor( 0.5, 0.5, 0.5, 0.5 );
			mSurface.SetShininess( 10.0 );
			mPS.gravity = 9.8;
			mPS.drag = 0.01;
			mPS.SetSpringConstant(5);
			mPS.SetSpringDampingConstant(1);
			mSurface.SetMassProportion(1); // proportional to numer of particles in x
			break;

		case 't':
			mSurfaceConditionsName = "Trampoline";
			mSurface.SetAmbientColor( 0.1, 0.1, 0.3, 1.0 );
			mSurface.SetDiffuseColor( 0.0, 0.0, 0.6, 1.0 );
			mSurface.SetSpecularColor( 0.5, 1.0, 1.0, 1.0 );
			mSurface.SetShininess( 70.0 );
			mPS.gravity = 9.8;
			mPS.drag = 0.001;
			mPS.SetSpringConstant(10);
			mPS.SetSpringDampingConstant(1);
			mSurface.SetMassProportion(0.5); // proportional to numer of particles in x
			break;

		case 'f':
			mSurfaceConditionsName = "Foam";
			mSurface.SetAmbientColor( 1.0, 1.0, 1.0, 1.0 );
			mSurface.SetDiffuseColor( 0.8, 0.8, 1.0, 1.0 );
			mSurface.SetShininess( 10.0 );
			mPS.gravity = 0.5;
			mPS.drag = 1.0;
			mPS.SetSpringConstant(1);
			mPS.SetSpringDampingConstant(1);
			mSurface.SetMassProportion(1); // proportional to numer of particles in x
			break;

		case 'c':
			mSurfaceConditionsName = "Clay";
			mSurface.SetAmbientColor( 0.6, 0.6, 0.4, 1.0 );
			mSurface.SetDiffuseColor( 0.3, 0.3, 0.2, 1.0 );
			mSurface.SetSpecularColor( 0.2, 0.2, 0.2, 1.0 );
			mSurface.SetShininess( 10.0 );
			mPS.gravity = 1.0;
			mPS.drag = 2.0;
			mPS.SetSpringConstant(0.5);
			mPS.SetSpringDampingConstant(1);
			mSurface.SetMassProportion(1); // proportional to numer of particles in x
			break;

		case 'j':
			mSurfaceConditionsName = "Jello";
			mSurface.SetAmbientColor( 1.0, 0.0, 0.0, 1.0 );
			mSurface.SetDiffuseColor( 1.0, 0.0, 0.0, 1.0 );
			mSurface.SetSpecularColor( 1.0, 1.0, 1.0, 1.0 );
			mSurface.SetShininess( 100.0 );
			mPS.gravity = 0;
			mPS.drag = 0.1;
			mPS.SetSpringConstant(10);
			mPS.SetSpringDampingConstant(2);
			mSurface.SetMassProportion(1); // proportional to numer of particles in x
			break;

		case 'w':
			mSurfaceConditionsName = "Waterbed";
			mSurface.SetAmbientColor( 0.4, 1.0, 0.2, 1.0 );
			mSurface.SetDiffuseColor( 0.1, 0.8, 0.5, 1.0 );
			mSurface.SetSpecularColor( 0.6, 0.6, 1.0, 1.0 );
			mSurface.SetShininess( 200.0 );
			mPS.gravity = 2;
			mPS.drag = 0.001;
			mPS.SetSpringConstant(1);
			mPS.SetSpringDampingConstant(1);
			mSurface.SetMassProportion(1); // proportional to numer of particles in x
			break;

		case '1':
			mSurfaceParticles = 3;
			ConstructSurface(mSurfaceParticles);
			break;

		case '2':
			mSurfaceParticles = 5;
			ConstructSurface(mSurfaceParticles);
			break;

		case '3':
			mSurfaceParticles = 10;
			ConstructSurface(mSurfaceParticles);
			break;

		case '4':
			mSurfaceParticles = 15;
			ConstructSurface(mSurfaceParticles);
			break;
		
		case '5':
			mSurfaceParticles = 20;
			ConstructSurface(mSurfaceParticles);
			break;
		
		case '6':
			mSurfaceParticles = 25;
			ConstructSurface(mSurfaceParticles);
			break;
		
		case '7':
			mSurfaceParticles = 30;
			ConstructSurface(mSurfaceParticles);
			break;
		
		case '8':
			mSurfaceParticles = 35;
			ConstructSurface(mSurfaceParticles);
			break;
		
		case '9':
			mSurfaceParticles = 40;
			ConstructSurface(mSurfaceParticles);
			break;
		
		case '0':
			mSurfaceParticles = 50;
			ConstructSurface(mSurfaceParticles);
			break;
		
		case 'n':
			mDrawNormals = !mDrawNormals;
			break;

		case 'i':
			mDrawInfo = (EDrawInfo)(mDrawInfo + 1);
			if (mDrawInfo >= drawInfo_PastEndOfList)
				mDrawInfo = drawInfo_StartOfList;
			break;

		case 's':
			mDrawSurface = !mDrawSurface;
			mPS.useDepthForMouseZ = mDrawSurface;
			break;

		case 'p':
			mDrawSystem = !mDrawSystem;
			break;

		case 'm':
			if (mPause)
				AdvanceTime(mManualTimeStep);
			break;

		case 'x':
			mUseFixedTimeStep = !mUseFixedTimeStep;
			break;
			
		case ' ':
			mPause = !mPause;
			break;

		case 'q':		// quit
			exit(0);
			break;

		default:
			//printf("Invalid menu choice: %c", option);
			break;
	}
}

void materialsTest(void)
{

    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat no_shininess[] = { 0.0f };
    GLfloat low_shininess[] = { 5.0f };
    GLfloat high_shininess[] = { 100.0f };
    GLfloat mat_emission[] = {0.3f, 0.2f, 0.2f, 0.0f};

    /* clear the window */
    glClearColor(0.0f,0.4f,0.4f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);



/*  draw sphere in first row, first column
 *  diffuse reflection only; no ambient or specular  
 */
    glPushMatrix();
    glTranslatef (-3.75, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in first row, second column
 *  diffuse and specular reflection; low shininess; no ambient
 */
    glPushMatrix();
    glTranslatef (-1.25, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in first row, third column
 *  diffuse and specular reflection; high shininess; no ambient
 */
    glPushMatrix();
    glTranslatef (1.25, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in first row, fourth column
 *  diffuse reflection; emission; no ambient or specular reflection
 */
    glPushMatrix();
    glTranslatef (3.75, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in second row, first column
 *  ambient and diffuse reflection; no specular  
 */
    glPushMatrix();
    glTranslatef (-3.75, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in second row, second column
 *  ambient, diffuse and specular reflection; low shininess
 */
    glPushMatrix();
    glTranslatef (-1.25, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in second row, third column
 *  ambient, diffuse and specular reflection; high shininess
 */
    glPushMatrix();
    glTranslatef (1.25, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in second row, fourth column
 *  ambient and diffuse reflection; emission; no specular
 */
    glPushMatrix();
    glTranslatef (3.75, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in third row, first column
 *  colored ambient and diffuse reflection; no specular  
 */
    glPushMatrix();
    glTranslatef (-3.75, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in third row, second column
 *  colored ambient, diffuse and specular reflection; low shininess
 */
    glPushMatrix();
    glTranslatef (-1.25, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in third row, third column
 *  colored ambient, diffuse and specular reflection; high shininess
 */
    glPushMatrix();
    glTranslatef (1.25, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

/*  draw sphere in third row, fourth column
 *  colored ambient and diffuse reflection; emission; no specular
 */
    glPushMatrix();
    glTranslatef (3.75, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glutSolidSphere(1.0,20,20);
    glPopMatrix();

    glFlush();
}

/*
 	Function:	HandleKeyboard
 	Usage:		HandleKeyboard(key, x, y)
 	---------------------------------------------------------------------------
 	Handles some useful keyboard shortcuts.
 */
void HandleKeyboard(unsigned char key, int x, int y)
{
	if (key == 27) key = 'q'; // escape
	HandleMenuCommand(key);
}

/*
 	Function:	HandleSpecialKeys
 	Usage:		HandleSpecialKeys(key, x, y);
 	---------------------------------------------------------------------------
 	Handles special keys, ie arrow keys.  Arrow keys perform similar functions
 	to left mouse dragging.
 */
void HandleSpecialKeys(int key, int x, int y)
{
	if((key == GLUT_KEY_LEFT || key == GLUT_KEY_UP ||
		key == GLUT_KEY_RIGHT || key == GLUT_KEY_DOWN))
	{
		float		dx, dy;					// ammount to move in x,y
		
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

		mSurfaceParticles += dy;
		if (mSurfaceParticles < 3) mSurfaceParticles = 3;
		ConstructSurface(mSurfaceParticles);		
	}
}


/*
 	Function:	HandleMouseButton
 	Usage:		HandleMouseButton(button, state, x, y);
 	---------------------------------------------------------------------------
	mouse press/release handler
	  
	   left button:		
	   		simulating:	pull at particle
	   		designing:	add particle/constraint
	   		
	   middle button:	unused
	   right button:	unused (mapped to pull-down menu)
*/
void HandleMouseButton(int button, int state, int x, int y)
{
	int part;	// index of a particle
	
	if (mDesign)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			switch (state)
			{
				case GLUT_DOWN:
					switch (mMode)
					{
						case designMode_Particle:
							mPS.AddParticle(x,y);
							break;
						case designMode_Nail:
							if ((part = mPS.GetClosestParticle(x, y)) != -1)
								mPS.AddNailConstraint(part);
							break;
						case designMode_Spring:
							if ((part = mPS.GetClosestParticle(x, y)) != -1)
								mCurrSpringConstraint = mPS.AddSpringConstraint(part, part);
							break;
					}
					break;

				case GLUT_UP:
					switch (mMode)
					{
						case designMode_Spring:
							if (mCurrSpringConstraint != NULL)
							{
								if ((part = mPS.GetClosestParticle(x, y)) != -1)
								{
									mCurrSpringConstraint->SetParticle(part);
									
									if (mCurrSpringConstraint->ParticlesAreValid())
									{
										// keep the constraint
									}
									else
									{
										mPS.DeleteConstraint(mCurrSpringConstraint);
									}
								}
								else
								{
									mPS.DeleteConstraint(mCurrSpringConstraint);
								}
								
								mCurrSpringConstraint = NULL;
							}
							break;
					}
					break;
			}
		}    
		else if (button == GLUT_MIDDLE_BUTTON)
		{ 
			switch (state)
			{
				case GLUT_DOWN:
					break;
				case GLUT_UP:
					break;
			}
		}
	}
	else
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			switch (state)
			{
				case GLUT_DOWN:
					mPS.ActivateMouseSpring(x, y);
					break;

				case GLUT_UP:
					mPS.DeactivateMouseSpring();
					break;
			}
		}    
	}
}

// mouse motion callback
void  HandleMouseMotion(int x, int y)
{
	if(!mDesign)
	{
		mPS.MouseMove(x, y);
	}
	else
	{
		if (mCurrSpringConstraint != NULL)
		{
			mCurrSpringConstraint->SetParticle(mPS.GetClosestParticle(x,y));
		}
	}
}

/*
 	Function:	Reshape
 	Usage:		glutReshapeFunc(Reshape); 
 	---------------------------------------------------------------------------
 	Callback for reshaping (resizing) the window.  Changes the projection
 	transformation.
 */
void Reshape(int width, int height)
{
	// glNewList(windowView, GL_COMPILE); // _AND_EXECUTE);
	/* change viewport dimensions */
	glViewport(0, 0, width, height);

	// remember the width and height
	mWindW = width;
	mWindH = height;

	DoProjectionTransformation();

    DoViewingTransformation();

    // sync haptics view to graphics
    UpdateHapticMapping();
}

/*
 	Function:	UpdateHapticMapping
 	Usage:		UpdateHapticMapping(); 
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
    
    /* fit haptic workspace to the bound of the deformable surface */
    hduVector3Dd minPt(-kSurfaceSize / 2.0, -kSurfaceSize / 2.0, -kSurfaceSize / 2.0);
    hduVector3Dd maxPt(kSurfaceSize / 2.0, kSurfaceSize / 2.0, kSurfaceSize / 2.0);
    hluFitWorkspaceBox(modelview, minPt, maxPt);

    /* compute cursor scale */
    mCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    const int CURSOR_SIZE_PIXELS = 20;
    mCursorScale *= CURSOR_SIZE_PIXELS;
}

/* redisplaying graphics */
void Display(void)
{
    hlBeginFrame();

    hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.1);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.8);

    hlCheckEvents();

	/* clear the display */
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//materialsTest();

	if (mDrawSurface)
		mSurface.DrawSurface();

    // draw the haptic surface
    hlHintb(HL_SHAPE_DYNAMIC_SURFACE_CHANGE, HL_TRUE);
    hlTouchableFace(HL_BACK);
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, mSurfaceShapeId);
        mSurface.DrawSurface();
    hlEndShape();
	
	glDisable(GL_LIGHTING);

	if (mDrawSystem)
		mPS.Draw();

	if (mDrawNormals)
		mSurface.DrawSurfaceNormals();

	glEnable(GL_LIGHTING);

    // draw the 3D cursor
    RedrawCursor();

	// show FPS
	if (mDrawInfo != drawInfo_None)
		DisplayInfo();

	// swap the double buffers
	glutSwapBuffers();  

    hlEndFrame();

}

void DisplayInfo(void)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

    glPushMatrix();
    glLoadIdentity();

	// switch to 2d orthographic mMode for drawing text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, mWindW, mWindH, 0);
	glMatrixMode(GL_MODELVIEW);
	
	glColor3f(1.0, 1.0, 1.0);

	int textRowDown = 0; // lines of text already drawn downwards from the top
	int textRowUp = 0; // lines of text already drawn upwards from the bottom

	if (mDrawInfo == drawInfo_All)
		DrawBitmapString(5, 20 + textRowDown * 15, GLUT_BITMAP_9_BY_15, "Surface Resolution: %d", mSurfaceParticles);

	if (mDrawInfo == drawInfo_All || mDrawInfo == drawInfo_FPS)
		DrawBitmapString(mWindW - 10 * 9, 20 + (textRowDown) * 15, GLUT_BITMAP_9_BY_15, "FPS: %4.1f", DetermineFPS());

	textRowDown++;
	
	if (mDrawInfo == drawInfo_All)
	{
		std::string stepLabel;
		
		if (mUseFixedTimeStep)
			DrawBitmapString(mWindW - 24 * 9, 20 + (textRowDown) * 15, GLUT_BITMAP_9_BY_15, "Fixed Step (sec): %1.3f" , mTimeStep);
		else
			DrawBitmapString(mWindW - 18 * 9, 20 + (textRowDown) * 15, GLUT_BITMAP_9_BY_15, "Step (sec): %1.3f" , mTimeStep);

		DrawBitmapString(5, mWindH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "%s", mSurfaceConditionsName.c_str());

		std::string displayingList;
		if (mDrawSurface) displayingList += "surface ";
		if (mDrawNormals) displayingList += "normals ";
		if (mDrawSystem) displayingList += "system ";
		if (displayingList.length() == 0) displayingList = "(nothing)";
		displayingList.insert(0, "Displaying: ");
		DrawBitmapString(mWindW - displayingList.length() * 9, mWindH - 10 - (textRowUp++) * 15, GLUT_BITMAP_9_BY_15, "%s", displayingList.c_str());

		DrawBitmapString(5, mWindH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "kd: %4.1f", mPS.GetSpringDampingConstant());
		DrawBitmapString(5 + 12 * 9, mWindH - 10 - (textRowUp++) * 15, GLUT_BITMAP_9_BY_15, "Gravity:%5.1f", mPS.gravity);
		
		DrawBitmapString(5, mWindH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "ks: %4.1f", mPS.GetSpringConstant());
		DrawBitmapString(5 + 12 * 9, mWindH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "Mass:   %5.1f", mSurface.GetMassProportion());
		DrawBitmapString(5 + 30 * 9, mWindH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "Drag: %5.3f", mPS.drag);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
	
	// turn depth and lighting back on for 3D rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}


/*
 	Function:	RedrawCursor
 	Usage:		RedrawCursor(); 
 	---------------------------------------------------------------------------
 Displays a cursor using the current haptic device proxy transform and the
 mapping between the workspace and world coordinates
 */
void RedrawCursor(void)
{
    static const double kCursorRadius = 0.5;
    static const int kCursorTess = 15;
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

    /* Apply the local position/rotation transform of the haptic device proxy */
    hlGetDoublev(HL_PROXY_TRANSFORM, proxytransform);
    glMultMatrixd(proxytransform);
        
    /* Apply the local cursor scale factor. */
    glScaled(mCursorScale, mCursorScale, mCursorScale);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(mCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}

double DetermineFPS(void)
{
	static unsigned int	lastMs = glutGet(GLUT_ELAPSED_TIME);
	unsigned int		currentMs = glutGet(GLUT_ELAPSED_TIME);
	unsigned int		framesMs = currentMs - lastMs;
	static unsigned int	framesCounted = 0;
	static double fps = 0;
	
	framesCounted++;

	#define DURATION_TO_TIME		1000	// duration in miliseconds for calculating fps

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

/*
 	Function:	DoViewingTransformation
 	Usage:		DoViewingTransformation(offset);
 	---------------------------------------------------------------------------
 	Viewing transformation goes from world coordinate to viewing coordinates
 	(world to camera).  This part of the transformation pipeline is applied
 	every time the scene is rendered.
 */
void DoViewingTransformation(void)
{
	float		target[] = {0, 0, 0};
	float		position[] = {0, kSurfaceSize / 3, -1.2*kSurfaceSize};
//	float		position[] = {0, 0, kSurfaceSize};
	//float		*position = mPS.particles[0].vx;

    glLoadIdentity();
	gluLookAt(	position[0], position[1], position[2],// position
				target[0], target[1], target[2],	// target
				0.0, 1.0, 0.0);						// up vector
}

void Idle(void)
{
	if (!mPause && !mDesign)
	{
		double t;

		if (mUseFixedTimeStep)
			t = mManualTimeStep;
		else
			t = GetTime();
		
		AdvanceTime(t);
	}

	glutPostRedisplay();
}

void AdvanceTime(double dt)
{
	static double prevTime = 0;
	double currTime = prevTime + dt;

	mPS.AdvanceSimulation(prevTime, currTime);
	mSurface.InvalidateVertexCache();

	mTimeStep = dt;
}

double GetTime(void)
{
	unsigned int	timeMs;
	unsigned int	timeSinceLastMs;
	static unsigned int timeOldMs = 0;
	
	timeMs = glutGet(GLUT_ELAPSED_TIME);
	timeSinceLastMs = timeMs - timeOldMs;
	timeOldMs = timeMs;
	
	if (timeSinceLastMs > kMaxStepSizeMs)
	{
		timeSinceLastMs = kMaxStepSizeMs;
	}

	return (double)timeSinceLastMs/ 1000;
}

/*
 	Function:	DoProjectionTransformation
 	Usage:		DoProjectionTransformation();
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
	gluPerspective(45,	// fov
					(float)mWindW/mWindH,	// aspect ratio
					6,	// near
					kSurfaceSize * 2);	// far
	glMatrixMode(GL_MODELVIEW);
}

/*
 	Function:	OnDragSurface
 	---------------------------------------------------------------------------
 	Event callback function called when moving device while touching surface.
 */
void HLCALLBACK OnTouchSurface(HLenum event, HLuint object, 
                              HLenum thread, HLcache *cache, 
                              void *userdata)
{
    if (event == HL_EVENT_TOUCH)
    {
        mPS.ActivateHapticDeviceConstraint();
    }
    else if (event == HL_EVENT_UNTOUCH)
    {
        mPS.DeactivateHapticDeviceConstraint();
    }
}

/*
 	Function:	OnDragSurface
 	---------------------------------------------------------------------------
 	Event callback function called when moving device while touching surface.
 */
void HLCALLBACK OnDragSurface(HLenum event, HLuint object, 
                              HLenum thread, HLcache *cache, 
                              void *userdata)
{
    
    hduVector3Dd proxyPos;
    hlCacheGetDoublev(cache, HL_PROXY_POSITION, proxyPos);

    hduVector3Dd contactNormal;
    hlCacheGetDoublev(cache, HL_PROXY_TOUCH_NORMAL, contactNormal);
    hduVector3Dd reactionForce;
    hlCacheGetDoublev(cache, HL_DEVICE_FORCE, reactionForce);
    const double deviceToParticleForce = 5.0;
    hduVector3Dd particleForce = 
        -deviceToParticleForce * contactNormal * reactionForce.magnitude();

    mPS.HapticDeviceMove(proxyPos, particleForce);
}
