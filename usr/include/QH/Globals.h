//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Globals.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the basic necessities. ie system function headers. 
// It also declares the DeviceParameters Class with it's STL list, which 
// is used through out the code
// 
//
//---------------------------------------------------------------------

#ifndef GLOBALS_H
#define GLOBALS_H


#pragma warning(disable:4996)//Get rid of depricated warinigs

#define RELEASE_uAPI 1

#define QUICK_HAPTICS_VERSION 1

#include "CompilerConfig.h"


#ifdef WIN32
	#ifndef WIN_INCLUDES_H
	#define WIN_INCLUDES_H
		#if defined(WIN32)
		#include <windows.h>
		#endif
	#endif
#elif defined(linux)
	#include<sys/time.h>//Compiling for Linux
#endif







#ifndef GL_INCLUDES_H
#define GL_INCLUDES_H
	#if defined(WIN32) || defined(linux)
	//#include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
	#elif defined(__APPLE__)
	//#include <GLUT/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
	#endif
#endif

#ifndef HL_INCLUDES_H
#define HL_INCLUDES_H
	#include <HL/hl.h>
	#include <HDU/hduMatrix.h>
        #include <HDU/hduMath.h>
	#include <HDU/hduQuaternion.h>
	#include <HDU/hduBoundBox.h>
	#include <HDU/hduError.h>
	#include <HLU/hlu.h>
#endif


#ifndef STL_H
#define STL_H
	#include<list>
	#include<vector>
#endif

#ifndef CASSERT_H
#define CASSERT_H
	#include <cassert>
#endif

#define PI 3.14159265

//DataStructure to hold basic device parameters
struct DeviceParameters
{
	HHD m_DeviceHandle;
	HHLRC m_RenderingContext;
	HDstring m_DeviceName;
};

extern DeviceParameters g_Device;
extern std::list<DeviceParameters> g_DeviceParametersList;
extern std::list<DeviceParameters>::iterator g_DeviceParametersIterator;

#define kPI 3.1415926535897932384626433832795
#define CURSOR_LIGHT_LIMIT 3


#endif







