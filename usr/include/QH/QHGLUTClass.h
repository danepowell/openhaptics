//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: QHGLUTClass.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: The QHGLUT Class interfaces with the OpenGL class and creates 
// a GLUT window for renderingscenes in the uAPI
//
//---------------------------------------------------------------------

#ifndef QHGLUTCLASS_H
#define QHGLUTCLASS_H


#include "ExportDef.h"
#include "Globals.h"
#include "Cursor.h"
#include "Sphere.h"
#include "DeviceSpace.h"
#include "QHRenderer.h"



#include <GL/glut.h>




#define uAPIGLUT 1



/*! \brief This a global function to start the rendering loops

This function sets the QuickHaptics rendering engine in motion. The function is invoked after defining all objects, effects and callbacks in the main/WinMain
function*/
QHAPI void qhStart(void);


/*! \brief This class defines  GLUT specific display parameters. 

This class handles all the GLUT specific calls. The class inherits from the QHRenderer class and calls functions in the base class.


*/
class QHAPI QHGLUT:public QHRenderer
{
		///Function to initialise QHGLUT
		void initQHGLUT(int, char*[]);
		///This function returns a string description of error codes
		static const char* getErrorCodeName(HDerror errorCode);

		friend void QHGLUTDisplayFunc(void);
		friend void QHGLUTReshapeFunc(int, int);
		friend void updateWorkspace(void);
		friend void QHGLUTIdleFunc(void); 
		friend void QHGLUTExitFunc(void);
		friend void QHGLUTTimerFunc(int);


		int m_WindowNumber;
		
		///Function pointer to the default QHGLUT display callback
		void (*DisplayFunction)(void);
		///Function pointer to the reshape callback
		void (*ReshapeFunction)(int, int);
		///Function pointer to the QHGLUT Idle function	
		void (*IdleFunction)(void);
		///Function pointer to the Exit function	
		void (*ExitFunction)(void);
		///Function pointer to the QHGLUT timer function
		void (*TimerFunction)(int);

		///This function prints the error into the console window
		void static QHGLUTErrorCatcher(void);

		/*! Search for window by name

		This function will search for the widow by "name". The name for the window can be set using the QHRenderer::setName function.*/
        static void checkAndSetTitle(void);

		void (*m_pGraphicLoopCallbackPointer)(void);
		


	public:
		
		
		/*! \brief This constructor allows the programmer to define the the GLUT window. 

		The parameters to be passed into this function are obtained from the void main(int argc, char *argv[]) declaration*/
		QHGLUT(int, char*[]);

		/*! \brief The search window function allows the programmer to search for a window by "Window Name". 
		
		The name is set using the QHRenderer::setName function. 
		This function is useable in the GLUT class as the QuickHaptics API does not support multiple windows in GLUT. */
		static QHGLUT* searchWindow(char* WindowName);

		/*! \brief This function defines a graphic callback that is invoked on every graphic frame.	
		
		There can exist only ONE graphic callback function per window. Multiple calls to the preDrawCallback function will schedule 
		the function declared last as the Graphics Callback*/
		void preDrawCallback(void (*m_pGraphicLoopCallbackPointer)(void));



       
		
};


/////////////////////////////////////////////////////////////
//Prototype glut callbacks. The user can also define his/her
//own callbacks by appropriately pointing the function pointers
//in the QHGLUT class
/////////////////////////////////////////////////////////////


void QHGLUTDisplayFunc(void);
void QHGLUTReshapeFunc(int, int);
void updateWorkspace(void);
void QHGLUTIdleFunc(void); 
void QHGLUTExitFunc(void);
void QHGLUTTimerFunc(int);

#endif




