//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Cylinder.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the Cylinder Class primitive with all associated 
// parameters.
//
//---------------------------------------------------------------------


#ifndef CYLINDER_H
#define CYLINDER_H


#include "ExportDef.h"
#include "Shape.h"


/*!\brief This class to loads  a Cylinder shape.

	Use this class to load a Cylinder. The class generates a display list for the cylinder and then
	supplies display list to the parent shape class.
*/


class QHAPI Cylinder: public Shape
{
		///Virtual Function to actually gererate the cone display list
		virtual void pureGLCylinder(HLdouble m_Radius, HLdouble m_Height, HLdouble m_ResolutionHeight,HLdouble m_ResolutionCircumference, hduVector3Dd m_Position);
		///Cylinder parameters
		HLdouble m_Radius;
		///Cylinder parameters
		HLdouble m_Height;
		///Cylinder parameters
		HLdouble m_ResolutionHeight;
		///Cylinder parameters
		HLdouble m_ResolutionCircumference;
		///Cylinder parameters
		HLdouble m_PositionX;
		///Cylinder parameters
		HLdouble m_PositionY;
		///Cylinder parameters
		HLdouble m_PositionZ;

	public:
		

	
		/*! \brief Default Constructor

		Radius = 0.5,\n
		Height = 1.0,\n
		Height resolution (Number of slices) = 10,\n
		Circumferential Resolution = 10 (Slices of pie)\n
		Center of circular base is at the origin.*/
		Cylinder(void);
		/*! \brief Constructor with Radius, Height and Resolution

		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = 
		Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base is at the origin.*/
		Cylinder(HLdouble m_Radius, HLdouble m_Height, HLdouble Resolution);
		/*! \brief Constructor with radius, resolution and position

		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = 
		Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base = user-defined*/
		Cylinder(HLdouble m_Radius, HLdouble m_Height, HLdouble m_ResolutionHeight, HLdouble m_ResolutionCircumference, HLdouble m_PositionX ,HLdouble m_PositionY,HLdouble m_PositionZ);
		


		/*! \brief The update functions act just like constructors, except that they can be invoked anywhere in the program. This is the default update function

		The update function is used to update the Cylinder. The default values for update are\n
		
		Radius = 0.5,\n
		Height = 1.0,\n
		Height resolution (Number of slices) = 10,\n
		Circumferential Resolution = 10 (Slices of pie)\n
		Center of circular base is at the origin.*/
		void update(void);

		/*! \brief The update functions act just like constructors, except that they can be invoked anywhere in the program. This update function
		allows the user to define the Radius, Height and resolution of the Cone

		The update function is used to update the Cylinder. The default values for update are\n
		
		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base is at the origin.*/
		void update(HLdouble m_Radius, HLdouble m_Height, HLdouble Resolution);

		/*! \brief The update functions act just like constructors, except that they can be invoked anywhere in the program. This update function
		allows the user to define the Radius, Height and resolution of the Cylinder as well as it's position

		The update function is used to update the Cone. The default values for update are\n
		
		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = 
		Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base = user-defined*/
		void update(HLdouble m_Radius, HLdouble m_Height, HLdouble m_ResolutionHeight, HLdouble m_ResolutionCircumference, HLdouble m_PositionX ,HLdouble m_PositionY,HLdouble m_PositionZ);

		/*! \brief This function is used to search for a Cylinder by a name and return a pointer to the Cylinder Object if found.
	
		The name is assigned by the QHRenderer::setName function. If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Cylinder* searchCylinder(char* CylinderName);
		/*! \brief This function is used to search for a cylinder object by a ShapeID and return a pointer to the Cylinder if found.
	
		A shapeID is assigned to every haptic object by OpenHaptics. This is a unique number identifying
		the object under a fixed rendering context. This function can be used in events like "touch" or "untouch" etc... 
		which can be setup to invoke the callback functions and pass the ShapeID.
		The search function with the ShapeID can then be used to return a pointer to the primitive shape.
		If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Cylinder* searchCylinder(HLuint CylinderID);

		~Cylinder(void);///Destructor for the cylinder

        ///This function returns the radius of the cylinder
        HLdouble getRadius(void);
        ///This function returns the height of the cylinder
        HLdouble getHeight(void);
};

#endif