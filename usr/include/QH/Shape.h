//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Shape.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: This class describes the "Shape" base class. This class contains 
// functions for common operations on primitive shapes such as all transformations,
// colors, texture binding etc..
//
//---------------------------------------------------------------------


#ifndef SHAPE_H
#define SHAPE_H




#include "ExportDef.h"
#include "Globals.h"
#include "TextureLoader.h"




void HLCALLBACK buttonDownCollisionThreadCallback(HLenum event, HLuint object, 
                                                  HLenum thread, HLcache *cache, 
                                                  void *userdata);
void HLCALLBACK buttonDownClientThreadCallback(HLenum event, HLuint object, 
                                               HLenum thread, HLcache *cache, 
                                               void *userdata);
void HLCALLBACK buttonUpCollisionThreadCallback(HLenum event, HLuint object, 
                                             HLenum thread, HLcache *cache, 
                                             void *userdata);


/*! \brief This is the base class for all primitives.

This class contains all basic parameters required for a primitive like it's transforms, 
rotation  and translation functions. Inaddition to this it contains several properties 
like the stiffness, damping friction etc..of the virtual object. It also controls the 
dragging function. i.e defines if a shape can be dragged and contains functions to 
allow for dragging a shape.
*/
class QHAPI Shape
{
		

		friend class QHRenderer;
		friend class TriMesh;
		friend class Cursor;
		friend class DeviceSpace;
		
		friend class Box;
		friend class Cone;
		friend class Cylinder;
		friend class Line;
		friend class Plane;
		friend class Sphere;
		friend class Text;
		friend class TriMesh;

		friend void HLCALLBACK buttonDownCollisionThreadCallback(HLenum event, HLuint object, 
                                                  HLenum thread, HLcache *cache, 
                                                  void *userdata);

		friend void HLCALLBACK buttonUpCollisionThreadCallback(HLenum event, HLuint object, 
                                             HLenum thread, HLcache *cache, 
                                             void *userdata);
	
	
		//Haptic m_Stiffness of the shape
		HLdouble m_Stiffness;
		///Which side the stiffness is applied
		HLenum m_StiffnessSide;
		
		///Haptic m_Damping present in the shape
		HLdouble m_Damping;
		///Which side the damping is applied
		HLenum m_DampingSide;

		///Is the model Spinning?
		bool m_RotationFlag;
	    ///spin velocity (normalised)
		HLdouble m_RotationVelocity;
		///spin Axis
		hduVector3Dd m_RotationAxis;

		///Is the model orbiting?
		bool m_RevolutionFlag;
		///Orbital velocity (normalised)
		HLdouble m_RevolutionVelocity;
		///Axis of orbit
		hduVector3Dd m_RevolutionAxis;
		///centre of the orbit
		hduVector3Dd m_RevolutionPoint;

		///Is the model being dragged?
		bool m_DraggingFlag;

		///Sets the type of touch
		HLenum m_TouchModel;
		///Sets the m_SnapDistance if the model is rendered as a constraint
		HLfloat m_SnapDistance;

		///Sets the static friction on the surface of a  shape
		HLfloat m_StaticFriction;
		///Sets the dynamic friction on the surface of a  shape
		HLfloat m_DynamicFriction;
		///Sets the face friction is present on (front or back or both)
		HLenum m_FrictionSide;
		
		///Sets the amount of force applied till the proxy pops through to the other side
		HLdouble m_popthroughForce;
		///Sets the face where the pop through setting is applied (Front or BAck)
		HLenum m_popthroughSide;

		///Which side has to be touched. HL_FRONT, HL_BACK, HL_FRONT_AND_BACK
		HLenum m_FrontOrBackTouch;
		///Object of the m_Texture loader class
		TextureLoader m_Texture;

		HLdouble m_RotationFreqMultiplier;

		//__int64 getMachineCycleCount(void);

		///Scaling and positioning Transform
		static hduMatrix m_InitialModelOrientationTransform;
		///Scaling, Centering transform
		hduMatrix m_ScalingCenteringTransform;
		

		///This variable tracks if the "orbits" and "spins" are frame rate dependent.
		bool m_FrameRateDependentFlag;

        ///This flag controls whether the uAPI will position the mode in 3D space of if the programmer will providea transform to do this.
        bool m_AutoPositioningFlag;

		char* m_TextureFilename;


       

			

	#ifdef WIN32 	// Compiling for Windows
		LARGE_INTEGER m_Frequency;
		///This variable controls the frame rate independent spin
		LARGE_INTEGER m_SpinCurrentTimeStamp;
		///This variable controls the frame rate independent spin
		LARGE_INTEGER m_SpinPreviousTimeStamp;

		///This variable controls the frame rate independent orbit
		LARGE_INTEGER m_OrbitCurrentTimeStamp;
		///This variable controls the frame rate independent orbit
		LARGE_INTEGER m_OrbitPreviousTimeStamp;
	#elif defined(linux)// Compiling for Linux
		///This variable controls the frame rate independent spin
		struct timeval m_SpinCurrentTime;
		///This variable controls the frame rate independent spin
		struct timeval m_SpinPreviousTime;
		
		///This variable controls the frame rate independent orbit
		struct timeval m_OrbitCurrentTime;
		///This variable controls the frame rate independent orbit
		struct timeval m_OrbitPreviousTime;
	#endif


		//This function applies the model's rotation transform to the OpenGL stack
		void InternalFnApplyTransform(void);
		//Function to rotate the model. This function computes the actual rotation Matrix
		void InternalFnSpinModelTransform(void);
		//This function returns the axis transform used to align the model to the global cocordinates.
		hduMatrix InternalFnGetAxesTransform(void);

		//This function returns the transform required to place the model infront of the camera.
		hduMatrix InternalFnGetModelFrustrumTransform(void);

		//Function to calculate the transform to center the object within the viewing frustrum.
		static void InternalFnPlaceWithinFrustrum(hduVector3Dd m_Eye,hduVector3Dd m_LookAt, hduVector3Dd m_UpVector, GLdouble NearClippingPlane, GLdouble FarClippingPlane, GLdouble m_FOV);

		//Sets the m_InitialTransformTrackerFlag variable to make InternalFnApplyTransform function compute the inital transform
		void InternalFnComputeInitialTransform(void);
		//Resets the m_InitialTransformTrackerFlag variable to make InternalFnApplyTransform function not compute the inital transform
		void InternalFnDoNotComputeInitialTransform(void);

		void InternalFnApplyOneTimeTransform(void);

		
		//Function to produce an OpenGL rendering
		void InternalFnDrawGraphics(void);
		//Function to draw the Shape haptically
		void InternalFnDrawHaptics(void);
		//This function performs the actual dragging by operating on the Shape's matrices
		void InternalFnDoDragging(void);

		//Function to perform the actual matrix transformations required to make the model revolve
		void InternalFnOrbitModelTransform(void);

		//User function to define the Shape Axes
		void InternalFnShapeAxes(GLfloat UpX, GLfloat UpY, GLfloat UpZ, GLfloat FrontX, GLfloat FrontY, GLfloat FrontZ);
		
		
		
		//This function either makes the uAPI automatically position the mesh or mae the programmer manually set the transform
        void ObsoleteFnSetAutoPositioning(bool AutoPositioningFlag);

        //This function sets Automatic transforms or otherwise for scene positioning
        static void ObsoleteFnSetSceneAutoPositioning(bool AutoPositioningFlag);

		//STL list to Hold the shapeIDs of all objects in the scene
		std::list<HLuint> m_HapticShapeList;
		//STL iterator for the m_HapticShapeList
		std::list<HLuint>::iterator m_HapticShapeIterator;

				//This function is used for dragging a shape, if it has been declared as draggable. It saves off the Modeltransform when the drag button is pressed
		void InternalFnSetStartDragShapeTransform(hduMatrix m_StartDragShapeTransform);
		//This function is used for dragging a shape, if it has been declared as draggable. It saves off the Proxy Transform
		void InternalFnSetStartDragProxyTransform(hduMatrix m_StartDragProxyTransform);

		//This function returns the model transform at the begning of the drag
		hduMatrix InternalFnGetStartDragShapeTransform(hduMatrix m_StartDragShapeTransform);
		//This function returns the proxy transform at the begning of the drag
		hduMatrix InternalFnGetStartDragProxyTransform(hduMatrix m_StartDragProxyTransform);
		
		//Sets the drag context. The drag context is a variable to save off the context to identify which device is involved in the drag operation
		void InternalFnSetDragContext(HHLRC m_DragContext);
		//This function returns the drag context. The drag context is a variable to save off the context to identify which device is involved in the drag operation
		HHLRC InternalFnGetDragContext(void);

		
		//Function to store off errors.
		void InternalFnStoreErrorString(char* ErrorString);
		//Function to retrieve error messages, A '0' value means thatthere are no more errors to retrieve.
		char* InternalFnRetrieveErrorString(void);

		


		

		


	protected:

		
        //The Initial transform has to applied only once 
		bool m_InitialTransformTrackerFlag;
        //The name of the shape
		char m_Name[256];
		//Set parameter of the shape bieng rendered
		void setHapticRenderingParameters(void);
		//The scale of the Shape
		GLdouble m_ShapeScale;
		//The scale of the Shape in place
		GLdouble m_ShapeScaleInPlace;
		//What is the Upvector for the model? (user defined in model corodinated)
		hduVector3Dd m_ShapeUpVector;
		//What is the direction of the Front of the object (user defined in model corodinated)
		hduVector3Dd m_ShapeFrontVector;
		//The highest x,y,z coordinates present in the model. Used for scaling and bound box calculation
		hduVector3Dd m_Highest;
		//The lowest x,y,z coordinates present in the model. Used for scaling and bound box calculation
		hduVector3Dd m_Lowest;
		//The center of the model. used for centering the model to the origin.
		hduVector3Dd m_Centroid;
		//The m_Highest absolute vales of any coordinate Present in the Shape
		GLdouble m_HighestAbsolute;
		//Variable to specify if the programmer has defined the axis of the Mesh
		bool m_UserDefinedAxesFlag;
		//This variable keeps track of the rotation of Axes required to align the shape to a programmer defined axis or the longest axis
		hduMatrix m_MajorAxisTransform;
		//Current Transform of the shape
		hduMatrix m_ModelTransform;
		//Current position of the shape
		hduVector3Dd m_Position;
		//Matrix to store off the shape transform at the start of the drag
		hduMatrix m_StartDragShapeTransform;
		//Matrix to store off the proxy transform at the start of the drag
		hduMatrix m_StartDragProxyTransform;
		//Variable to save off the context to identify which device is involved in the drag operation
		HHLRC m_DragContext;
		//Is the model visible haptically
		bool m_HapticallyvisibleFlag;
		//Is the model visible graphically
		bool m_GraphicallyvisibleFlag;
		//The current OpenGL displaylist number
		GLuint m_DisplayList;
		//The texture variable
		glTexture m_GLTextureVariable;
		struct ErrorStructure
		{
			char ErrorString[200];
			bool m_ErrorProcessedFlag;
		};
		//This is a list of all errors generated in loading the model
		std::list<ErrorStructure> m_ErrorList;
		//This is an iterator user to run through all the errors generated
		std::list<ErrorStructure>::iterator m_ErrorListIterator;
		//This the original automatically computed model tranfrom to position and scale the model 
		hduMatrix m_DefaultModelTransform;
		//This variable holds the color of the shape
		HLdouble m_ShapeColorR;
		//This variable holds the color of the shape
		HLdouble m_ShapeColorG;
		//This variable holds the color of the shape
		HLdouble m_ShapeColorB;
		//This variable holds the color of the shape
		HLdouble m_ShapeColorA;
        //This flag determined if all the subsequently declared objects in the scene are draggable or not.
        static bool m_SceneDraggableFlag;
        //This flag determines whether the whole scene has to be Autopositioned or not.
        static bool m_SceneAutoPositioningFlag;
        //This flag controls the mode of rendering Depth/Feedback buffer for the shape
        HLenum m_RenderMode;
        //This flag controls the mode of rendering Depth/Feedback buffer for the scene
        static HLenum m_GlobalRenderMode;
		//Function to place the model into the the centre of the view
		void internalFnPositionAndOrientShape(void);
		//Function to align the shape with the OpenGl axis. i.e The Longest axis of the model will become the x axis, the shortest will become the y axis and the medium will become the z axis.
		hduMatrix internalFnAxisAlignWithShape(void);

	public:	

         

		




		
		
		
		
		Shape* m_pShapePointer;
		//Shape Constructor
		Shape(void);
		//Shape Destructor
		~Shape(void);




		
		
		
		///This function returns true or false depending on if the model is haptically visible or not
		bool getHapticVisibility(void);
		///This function returns true or false depending on if the model is graphically visible or not
		bool getGraphicVisibility(void);
		///This function returns the unsigned display list index for the shape
		GLuint getDisplayList(void);
		///This function returns the texture variable associated with the shape
		glTexture getTexture(void);
		///This function returns the filename of the texture file loaded
		char* getTextureFilename(void);
		///Use this function to query the color of the model. Note: The function works if you have previous set the color value of the shape.
		void getShapeColor(GLdouble* R,GLdouble* G,GLdouble* B);
		///Get the boundbox axes (longest, medium, small) in model coordinates
		void getModelBoundBoxAxes(hduVector3Dd* LongestAxis,hduVector3Dd* MediumAxis,hduVector3Dd* ShortestAxis);
		///User function to read back the user define shape axes set using the function specifyShapeAxes
		void getShapeAxes(hduVector3Dd* m_UpVector, hduVector3Dd* FrontVector);
		//Use this function to return the transform of the Shape primitive
		hduMatrix getTransform(void);
		/*! \brief User function to query the shape rotation matrix

		This function is a subset of the getTransform function . It returns the rotation matrix with out the translation components.*/
		hduMatrix getRotation(void);
		/*! \brief User function to query the shape translation (position)

		This function returns the position of the shape as a vector of 3 doubles (hduVector3Dd)*/
		hduVector3Dd getTranslation(void);
		/*! \brief Function to return the "name" of the model. 
		
		The name of the mode is set using the setName function. The
		default name for any model is the name of the class of the primitive*/
		char* getName(void);




		/*! \brief This function sets the haptic visibility of a model
		
		TRUE - Visible\n
		FALSE - Invisible*/
		void setHapticVisibility(bool m_HapticallyvisibleFlag);
		/*! \brief This sets the graphic visibility of a model
		
		TRUE - Visible\n
		FALSE - Invisible*/
		void setGraphicVisibility(bool m_GraphicallyvisibleFlag);
		/*! \brief Set the haptic stiffness of the object (F="k"x + Bv)

		The values for stiffness are normalized from 0 to 1. With 1 representing the maximum stiffness 
		that can be stably rendered by the haptic device in use\n
		By default the stiffness will be applied on both the front and back surfaces of the shape*/
		void setStiffness(HLdouble ShapeStiffness);
		/*! \brief Set the haptic stiffness of the object (F="k"x + Bv)

		The values for stiffness are normalized from 0 to 1. With 1 representing the maximum stiffness 
		that can be stably rendered by the haptic device in use. \n
		The programmer can also specify which side of the shape the stiffness will apply to.\n
		Acceptable strings are "front" and "back" (case insensitive). In case of an improper match both
		sides of the stiffness will be applied on both sides of the shape*/
		void setStiffness(HLdouble ShapeStiffness, HDstring m_StiffnessSide);
		/*! \brief Set the haptic damping of the object (F=kx + "B"v)

		The values for damping are normalized from 0 to 1. With 1 representing the maximum damping 
		that can be stably rendered by the haptic device in use\n
		By default the damping will be applied on both the front and back surfaces of the shape*/
		void setDamping(HLdouble ShapeDamping);
		/*! \brief Set the haptic damping of the object (F=kx + "B"v)

		The values for damping are normalized from 0 to 1. With 1 representing the maximum damping 
		that can be stably rendered by the haptic device in use. \n
		The programmer can also specify which side of the shape the damping will apply to.\n
		Acceptable strings are "front" and "back" (case insensitive). In case of an improper match both
		sides of the stiffness will be applied on both sides of the shape*/
		void setDamping(HLdouble ShapeDamping, HDstring m_DampingSide);
		/*! \brief This function sets the haptic and graphic visibility of a Shape in one function

		By default all shapes are haptically and graphically visible*/
		void setVisible(bool m_HapticallyvisibleFlag, bool m_GraphicallyvisibleFlag);
		/*! \brief Set the rendering mode of the object, OpenHaptics terminology: Constraint or Contact rendering\n
		TRUE - Magnetic\n
		FALSE - Normal\n

		In addition to the normal mode of rendering a shape. This is an additional mode, which allows
		the programmer to set the shape as "magnetic". Turning this property on makes the shape feel sticky.
		Use this feature in simulations to create a sticky line for guiding a surgical tool.*/
		void setMagnetic(bool magnetic);
		/*! \brief Set the rendering mode of the object. Specify the snap distance. OpenHaptics terminology: Constraint or Contact rendering
		TRUE - Magnetic\n
		FALSE - Normal\n

		In addition to the normal mode of rendering a shape. This is an additional mode, which allows
		the programmer to set the shape as "magnetic". Turning this property on maked the shape feel sticky.
		Use this feature in simulations to create a sticky line for guiding a surgical tool.\n
		In addition to turning the shape magnetic, this over loaded function lets the programmer specify how close
		to the object the proxy must approach before it gets 'stuck' to the shape.\n
		Note: The SnapDistance MUST be smaller than the OpenGl workspace. Otherwise openhaptics will throw a 
		BoundBox error.*/
		void setMagnetic(bool magnetic, HLfloat SnapDistance);
        /*! \brief Set the rendering mode of the object, (Constraint or Contact)
		TRUE - Normal\n
		FALSE - Magnetic*/
		void setContact(bool contact);
		/*! \brief Sets default static friction values on the surface of a shape.
			
		Static Friction Coefficient = 0.5\n
		Dynamic Friction Coefficient = 0.5\n

		Here 0.5 represents the half maximum range of the haptic device in use.*/
		void setFriction(void);
		/*! \brief Sets specific friction values for a shape

		This function lets the programmer define friction values for a shape. However OpenHaptics by default applied the 
		friction property on both sides of the surface.
		0 <= m_StaticFriction <= 1\n
		0 <= m_DynamicFriction <= 1\n
		
		*/
		void setFriction(HLfloat m_StaticFriction, HLfloat m_DynamicFriction);
		/*! \brief Sets specific friction values for a shape

		The functions lets the programmer set all friction paremeters for a shape. Including
		the side.

		0 <= m_StaticFriction <= 1\n
		0 <= m_DynamicFriction <= 1\n

		m_FrictionSide = "front" or "back" (case insensitive)*/
		void setFriction(HLfloat m_StaticFriction, HLfloat m_DynamicFriction,HDstring m_FrictionSide);
		/*! \brief Sets the default popthrough value

		This function allows the programmer to set a popthrough value for a shape. A "popthrough" occurs when the programmer exerts above
		a particular threshold on a shape. This causes the proxy to fall through the surface creating a "popthrough".

		Default threshold = 0.5\n
		Here 0.5 indicated the normalised force value.\n
		1.0 being the max continuous force of the device.\n
		The popthrough effect is applied on both the front and back surface of the shape.\n
		Note: The Shape::setTouchableFace function has to be used to make the backface touchable*/
		void setPopthrough(void);
		/*! \brief Overloaded function for user specified popthrough value

		This function allows the programmer to set a popthrough value for a shape. A "popthrough" occurs when the user exerts above
		a particular threshold on a shape. This causes the porxy to fall through the surface creating a "popthrough".

		0.0 <= m_popthroughForce <= 1.0\n
		1.0 being the max continuous force of the device.\n
		The popthrough effect is applied on both the front and back surface of the shape.\n
		Note: The Shape::setTouchableFace function has to be used to make the backface touchable*/
		void setPopthrough(HLdouble m_popthroughForce);
		/*! \brief Overloaded function for user specified popthrough value and popthrough side

		This function allows the programmer to set a popthrough value for a shape. A "popthrough" occurs when the user exerts above
		a particular threshold on a shape. This causes the porxy to fall through the surface creating a "popthrough". In addition
		this overloaded version of the setPopthrough function allows the programmer to specify which face "front" or "back" will
		have the popthrough property.

		0.0 <= m_popthroughForce <= 1.0\n
		1.0 being the max continuous force of the device.\n
		m_popthroughSide = "front" or "back" (case insensitive)*/
		void setPopthrough(HLdouble m_popthroughForce, HDstring m_popthroughSide);
		/*! \brief Function to set which face of the shape is touchable, The default is the front face

		FrontOrback = "front", "back" , anything else => "frontandback"*/
		void setTouchableFace(HDstring FrontOrBack);
		/*! \brief Set a name for the shape

		This function allows the programmer to set a name for a shape. This function is usefult when the user want to reference the shape
		in another function or a callback function. In this case the "search" (Trimesh::searchTrimesh, Sphere::searchSphere etc...)
		functions can be used to return search the shapes and return a pointer to the programmer"*/
		void setName(char* IncomingName);
		/*! \brief Function to load and bind a graphics texture file to a shape.

		This function is used to load texture files from disk. Specify the path and file name using
		forward slashes eg. foodir/foo.jpg. The function supports many popular file formats: JPG, bmp, tga, gif and more*/
		int setTexture(HDstring Filename);
		/*! \brief This function is used to make the model spin in it's place.

		The set sping function updates the shape transform every frame to spin the model. By default the model spins on the Y axis
		at a rate of 9 degrees per second in the clockwise direction. The rotation of the model is frame rate independent unless explicitly set to be frame rate dependednt by 
		setFrameRateDependent function.*/
		void setSpin(void);
		/*! \brief This function is used to make the model spin in it's place, with axis, direction and rotation velocity specified

		The set sping function updates the shape transform every frame to spin the model.The rotation of the model is frame rate
		independent unless explicitly set to be frame rate dependent by setFrameRateDependent function.\n
		Direction = HL_CW (Clockwise) or HL_CCW (Counterclockwise)\n
		0 <= m_Velocity <= 1.0 (1.0 corresponds to 180 degrees per second)*/
		void setSpin(HLenum Direction,float m_Velocity, hduVector3Dd m_RotationAxis);

		/*! \brief This function is used to make the model spin in it's place, with axis, direction and rotation velocity specified

		The set sping function updates the shape transform every frame to spin the model.The rotation of the model is frame rate
		independent unless explicitly set to be frame rate dependent by setFrameRateDependent function.\n
		-1.0 <= m_Velocity <= 1.0 (1.0 corresponds to 180 degrees per second)*/
		void setSpin(float m_Velocity, hduVector3Dd m_RotationAxis);




		/*! \brief Function to make the object draggable

		This function sets the object as draggable by the cursor. Note: By default all objects in the scene are draggable.
		Use this function to to dynamically change an object to draggable in a callback or any other scenario*/
		void setDraggable(void);
		/*! \brief Function to make the object undraggable or static

		This function sets the object as NOT draggable by the cursor. Note: By default all objects in the scene are draggable.
		Use this function to to dynamically change an object to non-draggable in a callback or any other scenario. */
        void setUnDraggable(void);
		/*! \brief This function sets the all the shapes declared after calling this function as draggable or undraggable

		Use this static function to declare all shapes in the scene declared after a call to this function as draggable or 
		undraggable based on the status of the SceneDraggableFlag. To make all the shapes in the scene undraggable declare
		setSceneDraggable(false) before declaring any shapes.
		
		Note: This function cannot be used in any of the callbacks to make the scene undraggable*/
        static void setSceneDraggable(bool SceneDraggableFlag);
 		/*! \brief Default Function to cause the shape to revolve around the origin.

		This function causes the shape to move in a circular path around the origin. The default axis is the Y axis and the shape
		orbits with a default angular velocity of 9 degrees per second.*/
		void setOrbit(void);
		/*! \brief Function to cause the shape to revolve around a user specified point,axis with specified direction and velocity

		This is the detailed orbit function with:\n

		Direction: HL_CW or HL_CCW\n
		0 <= m_Velocity <= 1.0 here 1.0 corresponds to 180 degrees per second\n
		m_RevolutionAxis - Any axis\n
		m_RevolutionPoint - Any point*/
		void setOrbit(HLenum Direction,float m_Velocity, hduVector3Dd m_RevolutionAxis, hduVector3Dd m_RevolutionPoint);
		/*! \brief Function to cause the shape to revolve around a user specified point,axis with specified direction and velocity

		This is the detailed orbit function with:\n

		-1.0 <= m_Velocity <= 1.0 here 1.0 corresponds to 180 degrees per second\n
		m_RevolutionAxis - Any axis\n
		m_RevolutionPoint - Any point*/
		void setOrbit(float m_Velocity, hduVector3Dd m_RevolutionAxis, hduVector3Dd m_RevolutionPoint);
		/*! \brief User function to set the model tranform explicitly

		This is an advanced function that let the programmer set the transform for the shape manually. The matrix has to be of type "hduMatrix" */
		void setTransform(hduMatrix ModelTransformMatrix);
		/*! \brief Function to set a translation to the shape

		This is a function that allows the programmer to explicitly specify the relative change in position of the shape in OpenGL worldspace. */
		void setTranslation(hduVector3Dd Translation);
		/*! \brief Function to set a translation to the shape

		This is a function that allows the programmer to explicitly specify the relative change in position of the shape in OpenGL worldspace. */
		void setTranslation(HLdouble x, HLdouble y, HLdouble z);
        /*! \brief Function to set the rotation of the model about the origin explicitly.

		This function allows the programmer to specify a one time rotation of the model about an axis. The rotation is centered about the origin*/
        void setRotation(hduVector3Dd Axis, HLdouble Angle);
		/*! \brief Function to set the rotation of the model about it's current position explicitly.

		This function allows the programmer to specify a one time rotation of the model about an axis. The model is rotated about it's
		current position*/
		void setRotationInPlace(hduVector3Dd Axis, HLdouble Angle);
		/*! \brief Function to scale the shape in it's current position

		This function scales the model according to the scale input by the programmer. Note: Scaling the model this way may
		cause it to move away from the original position. For scaling in place use the setScaleInPlace function*/
		void setScale(GLdouble scale);

		/*! \brief Function to scale the shape

		This function scales the model in it's current position (ie no translation) according to the scale input by the programmer. */
		void setScaleInPlace(GLdouble scale);
		/*! Function to orient the shape according to auser defined coordinate system in modelspace

		This function allows the user to change the orientation of the model by specifying a new coordinate system for the shape.
		For example: The programmer can transform the native xyz axes in model coordinates to be aligned along a new set of specified
		xyz axes defined by this function. In doing so the programmer can make cange the orientation of the model.*/
		void specifyShapeAxes(GLfloat UpX, GLfloat UpY, GLfloat UpZ, GLfloat FrontX, GLfloat FrontY, GLfloat FrontZ);
		///This function makes all "spins" and "orbits" frame rate dependent.
		void setFrameRateDependent(void);
		///This function makes all "spins" and "orbits" frame rate independent. The default is independent (may cause troublesome haptic rendering on large meshes)
		void setFrameRateIndependent(void);
		///This function restores the ModelTransform matrix to it's original default value
		void setShapeToDefault(void);
		///This function colors the shape with a user defined color
		void setShapeColor(GLdouble m_R,GLdouble m_G,GLdouble m_B);
 	    ///This function sets the render mode for the haptic rendering for the shape as Feedback buffer
        void setRenderModeFeedback(void);
        /*! \brief This function sets the render mode for the haptic rendering for the shape as Depth buffer

		The depth buffer rendering, though more efficient than the feedback buffer sometimes causes a confusion in correctly identifying the shape IDs.
		The symptoms of this behavior are: While attempting to drag one object, one or more other objects are picked up.*/
        void setRenderModeDepth(void);
        ///This function sets the render mode for all objects declared after a call to this function to the feedback buffer
        static void setGlobalRenderModeFeedback(void);
        /* \brief This function sets the render mode for all objects declared after a call to this function to the depth buffer

		The depth buffer rendering though more efficient than the feedback buffer sometimes causes a confusion in correctly identifying the shape IDs.
		The following are the symptoms of this behavior. If while attempting to drag one object another object is picked up or more than one object
		gets picked up.*/
        static void setGlobalRenderModeDepth(void);



		/*! \brief This function will sets the mipmap render mode for textures

		If a true value is passed:\n
		GL_TEX_MAG_FILTER to GL_LINEAR (default mode)\n
		GL_TEX_MIN_FILTER to GL_LINEAR_MIPMAP_LINEAR (default mode)\n\n

		If a  false values is passed:\n
		GL_TEX_MAG_FILTER to GL_LINEAR\n
		GL_TEX_MIN_FILTER to GL_LINEAR\n\n
		Note: This is a static function and hence a call to this function will change the texture mode for ALL textures, loaded AFTER a call to this function*/

		static void setMipMapTextureSmooth(bool SmoothTexFlag);

		
};


#endif




