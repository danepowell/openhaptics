//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: QHRenderer.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the OpenGL interface class. This class is used as a glue layer
// to interface with Win32Class and GLUTClass. Essentially preserving the platform
// independence of the code.
// 
//
//---------------------------------------------------------------------

/*! \mainpage QuickHaptics microAPI

 QuickHaptics is a microAPI that makes it fast and easy to write new haptic applications or to add haptics to existing applications.\n\n

 Quickhaptics contains graphic and haptic function calls to encapsulate the logical steps for typical haptic applications, enabling rapid
 program design and deployment. These steps include:

 - Including geometry files from popular animation packages\n
 - Initializing the graphics environment\n
 - Initializing the haptics environment\n
 - Setting up scenes\n
 - Mapping haptic parameters to scene objects\n
 - Setting up responses to interactions\n

 Using default values for parameters, a programmer can create a viable scene without the need to specify camera location, device space parameters, or
 values for various shape properties
 */
 
#ifndef OPENGL_H
#define OPENGL_H



#include "ExportDef.h"
#include "Globals.h"
#include "Cursor.h"
#include "Sphere.h"
#include "Line.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Box.h"
#include "Plane.h"
#include "TriMesh.h"
#include "Text.h"
#include "DeviceSpace.h"
#include "QHRenderer.h"



#define ERROR_LIMIT 32


extern std::list<Sphere*> g_SphereList;///Sphere primitive list
extern std::list<Sphere*>::iterator g_SphereIterator;///Sphere primitive iterator

extern std::list<Line*> g_LineList;///Line primitive list
extern std::list<Line*>::iterator g_LineIterator;///Line primitive iterator

extern std::list<Cylinder*> g_CylinderList;///Cylinder primitive list
extern std::list<Cylinder*>::iterator g_CylinderIterator;///Cylinder primitive iterator

extern std::list<Cone*> g_ConeList;///Cone primitive list
extern std::list<Cone*>::iterator g_ConeIterator;///Cone primitive list

extern std::list<Box*> g_BoxList;///Box primitive list
extern std::list<Box*>::iterator g_BoxIterator;///Box primitive list

extern std::list<Plane*> g_PlaneList;///Plane primitive list
extern std::list<Plane*>::iterator g_PlaneIterator;///Plane primitive list

extern std::list<TriMesh*> g_TriMeshList;///Triangular mesh primitive list
extern std::list<TriMesh*>::iterator g_TriMeshIterator;///Triangular mesh primitve list

extern std::list<Text*> g_TextList;///Text primitive list
extern std::list<Text*>::iterator g_TextIterator;///Text primitive list



extern std::list<DeviceSpace*> g_DeviceSpaceList;///g_Device Space list. This list is mainly of use in dual or more phantom configurations
extern std::list<DeviceSpace*>::iterator g_DeviceSpaceIterator;///g_Device Space iterator

extern std::list<Cursor*> g_CursorList;
extern std::list<Cursor*>::iterator g_CursorIterator;






#define CURSOR_SIZE_PIXELS 20
static double gCursorScale;

/*! \brief The QHRenderer class is used to draw the scene haptically and graphically.

The QHRenderer class contains functions to render all the primitives and additionally functions for the camera, error handling etc. 

*/



class QHAPI QHRenderer
{
		friend void QHGLUTDisplayFunc(void);
		friend void QHGLUTReshapeFunc(int width, int height);
		friend void QHGLUTExitFunc(void);
		///draw the haptic spheres
		static void drawHapticSphere(HLuint ContextNumber);
		///draw the graphics spheres
		static void drawGraphicSphere(void);

		///draw the haptic Lines
		static void drawHapticLine(HLuint ContextNumber);
		///draw the graphics Lines
		static void drawGraphicLine(void);
		
		///draw the haptic cylinders
		static void drawHapticCylinder(HLuint ContextNumber);
		///draw the graphics cylinders
		static void drawGraphicCylinder(void);

		///draw the haptic cones
		static void drawHapticCone(HLuint ContextNumber);
		///draw the graphics cones
		static void drawGraphicCone(void);

		///draw the haptic Boxes
		static void drawHapticBox(HLuint ContextNumber);
		///draw the graphics Boxes
		static void drawGraphicBox(void);

		///draw the haptic Planes
		static void drawHapticPlane(HLuint ContextNumber);
		///draw the graphics Planes
		static void drawGraphicPlane(void);

		///draw the haptic Trimeshes
		static void drawHapticTriMesh(HLuint ContextNumber);
		///draw the graphics trimeshes
		static void drawGraphicTriMesh(void);

		///draw the haptic text
		static void drawHapticText(HLuint ContextNumber);
		///draw the graphics text
		static void drawGraphicText(void);
        ///This function draws the debug box
        static void drawDebugBox(void);

		///Set a standard white material;
		static void setStandardMaterial(void);

        

        
		
		///Variable to hold the Field of view
		double m_FOV;
		///The near clippin plane	
		double m_NearPlane;
		///The far clipping plane
		double m_FarPlane;
		///The aspect ratio
		double m_Aspect;
		///Variable that specified to switch to default camera or if the user wants to define his own camera
		bool m_ManualCameraFlag;
		///Location of the eye
		hduVector3Dd m_Eye;
		///What direction are we looking at?
		hduVector3Dd m_LookAt;
		///Hold the Up direction
		hduVector3Dd m_UpVector;
        ///flag to turn on or off debugworkspace 
        static bool m_DebugWorkSpaceFlag;
        

		
		///Structure to hold errors
		struct ShapeErrorStructure
		{
			char ErrorString[200];
			bool m_ErrorProcessedFlag;
		};

		///All the model loading/cursors eventually end up in this variable
		static std::list<ShapeErrorStructure> m_GlobalShapeErrorList;

       


		  ///This variable holds the bound box of the shape
        hduBoundBox3Dd m_BoundBox;

		
		///Background color for the display window
		static HLdouble m_R;
		///Background color for the display window
        static HLdouble m_G;
		///Background color for the display window
        static HLdouble m_B;
		///Background color for the display window
        static HLdouble m_A;
		///This variable helps in setting a tranform in the Shape class to center the object in the camera view of the first window
		static bool m_ModelOrientationTransformFlag;

		/*! \brief A new camera position if desired inplace of an old one requires a call to this function

		*/
		void activateNewCamera(void);

		static bool m_LightFlag[8];
		static hduVector3Dd m_LightPosition[8];
		static void ApplyLights(void);
		static GLfloat m_AmbientLight[4];
        static GLfloat m_DiffuseLight[4]; 
        static GLfloat m_SpecularLight[4];


		int WindowWidth;
		int WindowHeight;

		void initLighting(void);



		
        

	protected:

		//The name of the window
		char m_Name[256];
        //Min point of the haptic workspace box
        static HLdouble m_MinPoint[3];
        //Max point of the haptic workspace box
        static HLdouble m_MaxPoint[3];
        static bool m_UserDefinedWorkspaceFlag;
        //This variable holds the title of the window        
        char m_WindowTitle[512];
        //A house keeping variable that also holds the title of the window
        char m_TempWindowTitle[512];
        //Tracking the opengl lights available for lighting
        static unsigned int m_OpenGLLightTracker;
		//Set initial parameters. so that junk values don't creep in
		QHRenderer(void);






		
		/* \brief Internal Fn: This function accumulates all the errors of the primitive classes into one structure

		The internalFnErrorHarvester function is used internally to accumulate the errors from each shape primitives.
		The function accumulates all the errors into the LocalErrorStructure structure. The errors stored in this class
		are retrieved at a later point.*/
		static void internalFnErrorHarvester(void);
		/* \brief  Internal Fn: This function retuns error strings from each Cursor and TriMesh till there are no more errors. Then it returns 0

		This function is used internally by QuickHaptics at runtime to return error messages one at a time. 
		When there are no more messages left, the function returns a NULL.*/
		static char* internalFnErrorStringRetriever(void);
        /* \brief  Internal Fn: Error handling, This function retrieves the type of Rendering Error. Note: This is an internal function.
		
		This function is used by both by the Win32Class and GLUTClass however the Win32 class uses this function in a reduced functionality mode.
		In the Win32 Class this function is used only to detect and error and to retrieve the error code. The Win32 class contains it's own look up 
		for what message corresponds to the Error code. However the GLUTClass uses existing HDU function to print the error message to the console.*/
		static HLerror internalFnCatchError(void);
  		/* \brief Internal Fn: This function is used by the inherited QHGLUT and QHWin32 to render haptics

		The internalFnRenderHaptics function is an internal function used by QuickHaptics to perform haptic rendering.
		In addition to just shape rendering, this function also handles the force effects and callbacks.*/
		static void internalFnRenderHaptics(void);
		
		/* \brief Internal Fn: This function is used to render all the graphics in a scene

		Much like the internalFnRenderHaptics function, this function is used to render the graphics in the scene.
		This function also sets the background color (default: custard) and sets the default material properties
		for a Shape primitive.*/
		static void internalFnRenderGraphics(void);
		
		/* \brief Internal Fn: This function is used by QuickHaptics to draw the cursor in a scene

		The internalFnDrawCursor function calls the display list for rendering the cursor. In addition to this function also
		checks if the setBeacon function has been activated. This makes the coursor another source of light*/
		static void internalFnDrawCursor(void);
		
		/* \brief Internal Fn: Clean up at exit (used only for glut) for all others use destructor

		This function is used to clean up after the a GLUT window exists. A GLUT exit does not allow the class
		destructors to be called because GLUT exits with an exit(0) after calling it's own exit handler. This 
		function contains the necessary code to explicitly deinitialise HD and HL programs at GLUT exit.*/
		static void internalFnCleanUp(void);
		
		/* \brief Internal Fn: Function to update the workspace of the haptic device 

		This function is used to update the workspace mapping between the haptic device and the OpenGL 
		workspace. The function also scales the default cursor appropriate to the workspace.*/
		static void internalFnUpdateWorkspace(void);
		
		
		/* \brief Internal Fn: Initialise OpenGL
		This function is used to initialise the openGL context and set the default lighting properties*/
		void internalFnInitGL(void);
		/* \brief Internal Fn: Function to set the OpenGL camera
		This function is used to update the camera when the user resizes the viewing window*/
		void internalFnUpdateCamera(int width,int height);

	public:
		
	
        /*! \brief This function sets the Haptic workspace to a user defined box

		Use this function to adjust the haptic workspace (the volume in which the cursor moves).  By defining the diagonal vertices of the cube
		in which the movement of the cursor is bounded the user can control which parts of the scene can be reached by the haptic device.*/
        static void setHapticWorkspace(HLdouble MinPoint[3], HLdouble MaxPoint[3]);

        ///Not implemented - Reserved.
        static void setDebugWorkspace(bool DebugWorkspaceFlag);

        /*! \brief This function sets the title of the display window*/
        void setWindowTitle(char* WindowTitle);

		/*! \brief This function will disable the specified Light Number (0-7)

		OpenGL supports a maximum of eight lights from 0 - 7. Specifying any other value will cause the function to
		return false. Note: The default light for the QuickHaptics API is light number 0.
		*/
		bool disableLight(GLuint LightNumber);
		/*! \brief This function will enable the specified Light Number (0-7)

		OpenGL supports a maximum of eight lights from 0 - 7. Specifying any other value will cause the function to
		return false. Note: The default light for the QuickHaptics API is light number 0. If the light parameeters have not been set beforehand 
		by default lights 1-7 will take the following values.\n
		AmbientLight = 0.0,0.0,0.0,1.0\n
		DiffuseLight = 0.5,0.5,0.5,1.0\n
		SpecularLight = 0.5,0.5,0.5,1.0\n
		Position = 0.0,0.0,0.0\n

		Light 0: Being the default this light always follows the location of the camera. Hence this light can only be turned on or off.
		It's position cannot be changed.
		*/
		bool enableLight(GLuint LightNumber);
		/*! \brief This function sets the specified light at the desired XYZ coordinates.

		1 <= Lightnumber <= 7\n
		The QuickHaptics API allows the programmer to set user defined lights at different points in the workspace. As OpenGL limits the maximum
		number of lights in a scene to eight and one light is occupied by the default light. The programmer can manipulate the other seven lights.
		Trying to manipulate any other light number ie greater than 7 or less than 1 will return a false value.*/
        bool setLight(GLuint LightNumber,GLdouble PositionX, GLdouble PositionY, GLdouble PositionZ);
        /*! \brief This function call need to be made before any call to the setLightFunction to set the Ambient value for the scene.

		Default: AmbientLight = 0.0,0.0,0.0,1.0*/
        void setLightAmbient(GLdouble R,GLdouble G, GLdouble B, GLdouble A);
        /*! \brief Create a new lightsource properties - Diffuse

		Default: DiffuseLight = 0.5,0.5,0.5,1.0*/
        void setLightDiffuse(GLdouble R,GLdouble G, GLdouble B, GLdouble A);
        /*! \brief Create a new lightsource properties - Specular

		Default: SpecularLight = 0.5,0.5,0.5,1.0*/
        void setLightSpecular(GLdouble R,GLdouble G, GLdouble B, GLdouble A);
		/*! \brief Function to set camera parameters
		
		The setCamera function us a user function which lets the user define a camera view. The function takes as input parameters the \n 
		Field Of View - FOV \n
		Distance to NearPlane \n
		Distance to FarPlane \n
		The Aspect Ratio \n
		The Eye Position \n
		The Look At Point \n
		The Up Vector*/
		void setCamera(float FOV, float NearPlane, float FarPlane, hduVector3Dd Eye,hduVector3Dd LookAt,hduVector3Dd UpVector);
		/*! \brief This function gives a name to the QHWin32 or QHGLUT class.

		Use this function when you want to access class functions without
		making the variable global. 
		Use this function to give a searchable name to the object.*/
		void setName(char* IncomingName);
		/*! \brief User Fn: Set the background color for the OpenGL window
		
		This function is used to set the background color of the workspace. The default is a "custard" color.
		The user can however set the background color by setting the Red, Green and Blue values*/
		static void setBackgroundColor(HLdouble Red,HLdouble Green,HLdouble Blue);
		 /*! \brief  Internal Fn: This function sets the default camera explictly if invoked by the user.

		The  function parses the shape pointer lists and calculates the appropriate camera location that can encompass all the
		objects in the scene. The function generates bound boxes for all primitives and then find the global bound box containing all the 
		sub-boundboxes. Based on this global bound box the function then determines the location of the camera. For more information about
		the camera positioning in QuickHaptics please refer to the manual.*/
        void setDefaultCamera(void);

		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(Sphere*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(Line*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(Cylinder*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(Cone*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(Box*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(Plane*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(TriMesh*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the primitives to display
		void tell(Text*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the devices in operation
		void tell(DeviceSpace*);
		///This function is required to tell the current rendering system (Win32/GLUT) about the cursors to be rendered
		void tell(Cursor*);

        /*! \brief This function is used to query the camera parameters.

		A user can use this function to query camera parameters from Quickhaptics. Use this function to query camera parameter and then modify them. 
		For example the user can query the camera parameters set by the default camera and then figure out what parameters must be user for a left, 
		right ot top view.*/
		void getCamera(float* FOV, float* NearPlane, float* FarPlane,hduVector3Dd* Eye,hduVector3Dd* LookAt,hduVector3Dd* UpVector);

		/*! \brief Use this function to get the transformation from the OpenGL worldspace to the Device Space.

		In certain cases like while using the servoLoop within QuickHaptics it becomes necessary to convert coordinates from the worldspace to device space. 
		This function returns the current transformation between world and devicespace.*/
        hduMatrix getWorldToDeviceTransform(void);


		
};


/*! \brief Use this function to get the version number of the QuickHaptics API*/
QHAPI char* getVersion(void);

#endif
