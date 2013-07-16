//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Plane.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the Plane Class primitive with all associated 
// parameters.
// 
//
//---------------------------------------------------------------------

#ifndef PLANE_H
#define PLANE_H


#include "ExportDef.h"
#include "Shape.h"


/*!\brief This class to loads  a Plane shape.

	Use this class to load a Plane. The class generates a display list for the Plane and then
	supplies display list to the parent shape class.
*/

class QHAPI Plane: public Shape
{
		virtual void pureGLPlane(HLdouble m_Width, HLdouble m_Height, hduVector3Dd m_Position);

		///Plane parameters
		HLdouble m_Width;
		///Plane parameters
		HLdouble m_Height;
		
		///Plane parameters
		HLdouble m_PositionX;
		///Plane parameters
		HLdouble m_PositionY;
		///Plane parameters
		HLdouble m_PositionZ;
	public:
		
	
		/*! \brief Default Constructor

		The parameters to this constructor are:\n
		Length = 0.5\n
		Width = 0.5\n
		Position of center of plane = (0.0, 0.0, 0.0)\n
		Note: If you create a scene with ONLY ONE plane parallel to the XY plane, the plane will not be visible.
		The camera will not orient itself to this plane because the bound-box generated will be 2D. The best solution 
		to this problem is to put a very small Sphere at the center of the plane.*/
		Plane(void);
		/*! \brief Constructor with width and height of the plane specified
		
		The parameters to this constructor are:\n
		Length = user-defined\n
		Width = user-defined\n
		Position of center of plane = (0.0, 0.0, 0.0)\n
		Note: If you create a scene with ONLY ONE plane parallel to the XY plane, the plane will not be visible.
		The camera will not orient itself to this plane because the bound-box generated will be 2D. The best solution 
		to this problem is to put a very small Sphere at the center of the plane.*/
		Plane(HLdouble m_Width, HLdouble m_Height);
		/*! \brief Constructor with width, height and position of the plane specified.

		The parameters to this constructor are:\n
		Length = user-defined\n
		Width = user-defined\n
		Position of center of plane = user-defined\n
		Note: If you create a scene with ONLY ONE plane parallel to the XY plane, the plane will not be visible.
		The camera will not orient itself to this plane because the bound-box generated will be 2D. The best solution 
		to this problem is to put a very small Sphere at the center of the plane.*/
		Plane(HLdouble m_Width, HLdouble m_Height, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble m_PositionZ);
		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program.
		This is the default update function

		The update function is used to update the Plane. The default values for update are:\n
		Length = 0.5\n
		Width = 0.5\n
		Position of center of plane = (0.0, 0.0, 0.0)*/
		void update(void);
		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program.
		This update function accepts the width and height

		The update function is used to update the Plane. The default values for update are:\n
		Length = user-defined\n
		Width = uder-defined\n
		Position of center of plane = (0.0, 0.0, 0.0)*/
		void update(HLdouble m_Width, HLdouble m_Height);

		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program.
		This update function accepts the width and height and the position.

		The update function is used to update the Plane. The default values for update are:\n
		Length = user-defined\n
		Width = uder-defined\n
		Position of center of plane = user-defined*/
		void update(HLdouble m_Width, HLdouble m_Height, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble m_PositionZ);

		/*! \brief This function is used to search for a Plane by a name and return a pointer to the Plane Object if found.
	
		The name is assigned by the QHRenderer::setName function. If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Plane* searchPlane(char* PlaneName);
		/*! \brief This function is used to search for a Plane object by a ShapeID and return a pointer to the Plane if found.
	
		A shapeID is assigned to every haptic object by OpenHaptics. This is a unique number identifying
		the object under a fixed rendering context. This function can be used in events like "touch" or "untouch" etc... 
		which can be setup to invoke the callback functions and pass the ShapeID.
		The search function with the ShapeID can then be used to return a pointer to the primitive shape.
		If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Plane* searchPlane(HLuint PlaneID);

		///Plane destructor
		~Plane(void);

        ///This function returns the dimensions of the plane. Note: Only Length and Width are returned the third value is a zero
        hduVector3Dd getLengthWidth(void);
};

#endif