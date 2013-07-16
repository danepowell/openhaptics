//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Cursor.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the Cursor class. This class
// is used to represent the cursor on screen as well as
// to load any mesh objects as the cursor
//
//---------------------------------------------------------------------

#ifndef CURSOR_H
#define CURSOR_H


#include "ExportDef.h"
#include "Globals.h"
#include "TriMesh.h"


/*! \brief The Class cursor deals with the representation of the proxy/Haptic Interface Point in the graphical workspace.

This class generates a display list for the cursor. The cursor can be a default cursor or an userdefined one.
This class also allows for loading a triangluar mesh as the cursor.
*/



class QHAPI Cursor
{
	
		///Triangular mesh Object. This is used for loading a TriMesh shape superimposed over the cursor
		TriMesh* m_pCursorMesh;


		///Cursor relative to the origin of the model
		hduVector3Dd m_CursorRelativePosition;
		///Axis of rotation if a Moel is superimposed on the cursor and the user want it have a rotation.
		hduVector3Dd m_CursorAxis;
		///Angle by which to rotate about m_CursorAxis
		GLfloat m_CursorAngle;
		///Scaling factor for the Cursor
		GLfloat m_CursorScale;
		///Filename of the 3D mesh to be superimposed on the cursor
		char* m_pCursorFileName;


		
		///Function to create the cursor
		virtual void createCursor(void);
		///Optional "m_Name" for a cursor
		char m_CursorName[256];
		///Cursor display list
		GLuint m_CursorList;
		///The user can define the Up and Front directions of the 3d Mesh loaded on the cursor. This is a variable that keeps track of whether user has chosen to define the Model Orientation.
		bool m_UserDefinedAxesFlag;
		///This variable says is a model is being renderred of the default cursor.
		bool m_MeshCursorFlag;

		///Holds the current cursor transform
		hduMatrix m_CursorTransform;
		///Holds the current Scaling and centering for the cursor
		hduMatrix m_CursorScalingTransform;
		///Holds the m_CursorPosition
		hduVector3Dd m_CursorPosition;
		///This variable hold the transform of the model after it has be rotated to alignment with the cursor
		hduMatrix m_MajorAxisTransform;
		///Function to align the model with the cursor or a user defined Up and front direction
		void AxisAlignCursorWithShape(void);
		///This variable controls if the Cursor is in m_DebugFlag mode or Normal mode
		bool m_DebugFlag;

		GLuint m_DebugCursorList;

        bool m_UserProxyTransformFlag;
        hduMatrix m_UserProxyTransform;
        hduMatrix m_AutomaticProxyTransform;
        ///This variable hold the gimabal angles
        hduVector3Dd m_GimbalAngles;
        ///This variable holds the current pinch value
        HLdouble m_PinchValue;
        ///This variable states whenter the cursor is graphically visible or not.
        bool m_GraphicallyVisible;

		friend class QHRenderer;
		///Function to draw the cursor
		void draw(HLdouble m_CursorScale, HLdouble ProxyTransform[16]);
		///This function sets the pinch value for one frame before it is updated automatically.
        void internalSetPinchValue(HLdouble PinchValue);
		///Function used to update the gibal angles perframe
        void setGimbalAngles(hduVector3Dd GimbalAngles);
	public:
		
		
		///Default constructor for default cursor
		Cursor(void);
		///Cursor for a mesh based cursor
		Cursor(char* m_FileName);
		///Destructor for clean up at exit
		~Cursor(void);

		
		
		
		
		///Function to change size of the TriMesh cursor
		void scaleCursor(GLfloat scale);
		/*! \brief Use this function to change the position of the cursor within a model

		Use this function to specify the displacement of the cursor from the origin in model coordinates*/
		void setRelativeShapePosition(GLfloat x, GLfloat y, GLfloat z);
		/*! \brief Use this function to change the orientation of the of the model relative to the cursor

		Use this function to specify the change in orientation of the of the model relative to the cursor, Specify the angle in degrees*/
		void setRelativeShapeOrientation(GLfloat VectorX, GLfloat VectorY, GLfloat VectorZ, GLfloat Angle);
		/*! \brief EXPERIMENTAL!! The programmer can use this function to define the orientation of the model

		This function with rotate the model in such a manner as to align the model 'Y' axis with the user specified 
		UP vector and also align the model 'Z' axis with the user specified 'Front' vector.*/
		void specifyShapeAxes(GLfloat UpX, GLfloat UpY, GLfloat UpZ, GLfloat FrontX, GLfloat FrontY, GLfloat FrontZ);
        /*! \brief The programmer can use this function to indicate to the Quickhaptics uAPI the PROGRAMMER will be updating the proxy transform. 
		
		If invoked then subsequent to this function call QuickHaptics will no longer update the proxy transform*/
        void setUserDefinedTransform(void);
        /*! \brief The programmer can use this function to indicate to the Quickhaptics uAPI the PROGRAMMER will be updating the proxy transform
		In this overloaded form the user has to supply the proxy transformation matrix.*/
        void setUserDefinedTransform(hduMatrix m_UserProxyTransform);
        /*! \brief The programmer can use this function to make the Quick Haptics uAPI update the proxy transform. 

		Note: By default the QuickHaptics uAPi updated the proxy transform. Use this function to switch back to the automatic update feature
		if Cursor::setUserDefinedTransform function has been invoked at an earlier instance.*/
        void setAutomaticTransform(void);
       

		/*! \brief This is a default function that can be used to display a TriMesh teapot.*/
		void update(void);

        /*! \brief This function can be used if the programmer wishes to change the model loaded at runtime. 

		In case the programmer wishes to change the model at runtime.\n
		Note: The process for parsing a file from disk may be slow depending on the size of the file. Please keep this in mind while using the update
		function*/
		void update(char* m_FileName);

		/*! \brief Function to search the cursor by "name" returns a pointer to the cursor if the specified name and the name of the cursor match. Otherwise return 0

		Sometime it is necessary to obtain a pointer to the cursor at runtime. The pointer can be user to invoke other cursor
		function that are required for properly implementing the application. This function returns a pointer to the cursor by "Name".
		the name for the cursor is set using the Cursor::setname("") function.*/
		static Cursor* searchCursor(char *m_CursorName);
		/*! \brief Function to set a "name" to the cursor

		The QuickHaptics uAPI provides a feature allowing the user to set a "name" to the cursor. The same "name" when invoked in conjunction with 
		the searchCursor command will allow return to the user, a pointer to the cursor object.*/
		void setName(char* IncomingName);
        

		///This function returns the Transfom of the Cursor
		hduMatrix getTransform(void);
		///This function returns the position of the Cursor.
		hduVector3Dd getPosition(void);
        ///This function returns the current gimbal angles of the haptic device
        hduVector3Dd getGimbalAngles(void);
        /*! \brief This function returns the current pinch value

		Note: The pinch value applies only to the 6DoF devices with pinch and scissor capability.*/
        HLdouble getPinchValue(void);

		/*! \brief The debugCursor functionality makes the mesh loaded on the cursor translucent and displays the default cursor*/
		void debugCursor(void);


		/* \brief Returns a pointer to the TriMesh 3D model embedded in the Cursor class.
		
		The cursor class is capable of loading a 3D mesh as the cursor (refer to Cursor constructor). This function returns a pointer to the TriMesh
		class object within the Cursor class. In case the default cursor is being used the function returns a NULL*/
		TriMesh* getTriMeshPointer(void);
        /*\brief Returns the name of the file used for loading a 3D mesh object. In case the default cursor is being used this function returns a NULL*/
        char* getFileName(void);

        /*\brief Sets the cursor visible or invisible.

		The programmer can use this function to set the cursor (default or 3D mesh) invisible by calling the function with a false value. The cursor
		can be turned back on again by passing a true value to the function.*/
		void setCursorGraphicallyVisible(bool GraphicallyVisible);

};


#endif
