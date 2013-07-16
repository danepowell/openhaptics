#pragma once
#define _CRT_SECURE_NO_DEPRECATE 1

#ifndef MAIN_H
#define MAIN_H

#if defined(WIN32)
#include <windows.h>
#include <conio.h>
#elif defined(linux)
#include<curses.h>
#include<stdlib.h>
#define _getch getch
#endif

#include <GL/glut.h>

#include <HL/hl.h>
#include <HDU/hdu.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>

#include <HDU/hduVector.h>
#include <HDU/hduMath.h>
#include <HDU/hduQuaternion.h>
#include <HDU/hduMatrix.h>

#include "Mass.h"
#include "Spring.h"

int winW = 800;
int winH = 600;

/* Haptic device and rendering context handles. */
static HHD ghHD = HD_INVALID_HANDLE;
static HHLRC ghHLRC = 0;

//Haptic shape id for the cloth. 
HLuint clothShapeId;

//Haptic Cursor defs
#define CURSOR_SIZE_PIXELS 30
static double gCursorScale;
static GLuint gCursorDisplayList = 0;

HLdouble pinchValue;		//Variable to store pinch value

bool resetSimulation = false;

hduVector3Dd proxyPosition;

static int massTouched = 0;	//Mass touched/ grabbed by user
static hduVector3Df force;	//Force applied by user
bool touchedCloth = false;	
bool movingOnCloth = false;

//All Function prototypes
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
bool runCalbrationRoutine();

void initCloth();
void setupCloth();
void initAllMass();
void initAllSprings();
void updatePhysics(double tPrev, double tCurr);
void updateNormals();
void drawCloth();
void drawMass();
void drawSprings();
void drawPinch();
int getClosestMass(hduVector3Dd& pos);

double determineFPS(void);
void advanceTime(double dt);
double getTime(void);
void drawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...);
void displayInfo();

void HLCALLBACK hlTouchCB(HLenum event, HLuint object,
						   HLenum thread, HLcache *cache,
						   void *userdata);

void HLCALLBACK hlUnTouchCB(HLenum event, HLuint object,
						   HLenum thread, HLcache *cache,
						   void *userdata);

void HLCALLBACK hlMotionCB(HLenum event, HLuint object,
						   HLenum thread, HLcache *cache,
						   void *userdata);


#endif	//MAIN_H
