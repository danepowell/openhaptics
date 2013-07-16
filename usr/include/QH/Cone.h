//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Cone.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the Cone Class primitive with all associated 
// parameters.
//
//---------------------------------------------------------------------

#ifndef CONE_H
#define CONE_H


#include "ExportDef.h"
#include "Shape.h"


/*!\brief This class to creates a Cone primitive.

	Use this class to load a Cone. The class generates a display list for the cone and then
	supplies display list to the parent shape class.
*/

class QHAPI Cone: public Shape
{
	///Function to actually gererate the cone display list
	virtual void pureGLCone(HLdouble m_Radius, HLdouble m_Height, HLdouble m_ResolutionHeight,HLdouble m_ResolutionCircumference, hduVector3Dd m_Position);
	///Cone parameters
	HLdouble m_Radius;
	///Cone parameters
	HLdouble m_Height;
	///Cone parameters
	HLdouble m_ResolutionHeight;
	///Cone parameters	
	HLdouble m_ResolutionCircumference;
	///Cone parameters
	HLdouble m_PositionX;
	///Cone parameters
	HLdouble m_PositionY;
	///Cone parameters
	HLdouble m_PositionZ;

	public:
		
		/*! \brief Default Constructor

		Radius = 0.5,\n
		Height = 1.0,\n
		Height resolution (Number of slices) = 10,\n
		Circumferential Resolution = 10 (Slices of pie)\n
		Center of circular base is at the (0.0,0.0,0.0)*/
		Cone(void);
		/*! \brief Constructor with Radius, Height and Resolution

		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base = user-defined*/
		Cone(HLdouble m_Radius, HLdouble m_Height, HLdouble Resolution);
		/*! \brief Constructor with radius, resolution and position

		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = 
		Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base = user-defined*/
		Cone(HLdouble m_Radius, HLdouble m_Height, HLdouble m_ResolutionHeight, HLdouble m_ResolutionCircumference, HLdouble m_PositionX ,HLdouble m_PositionY,HLdouble m_PositionZ);

		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program. This is the default update function

		The update function is used to update the Cone. The default values for update are\n
		
		Radius = 0.5,\n
		Height = 1.0,\n
		Height resolution (Number of slices) = 10,\n
		Circumferential Resolution = 10 (Slices of pie)\n
		Center of circular base is at the origin.*/
		void update(void);

		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program. This update function
		allows the user to define the Radius, Height and resolution of the Cone

		The update function is used to update the Cone. The default values for update are\n
		
		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base is at the origin.*/
		void update(HLdouble m_Radius, HLdouble m_Height, HLdouble Resolution);
		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program. This update function
		allows the user to define the Radius, Height and resolution of the Cone as well as it's position

		The update function is used to update the Cone. The default values for update are\n
		
		Radius = user-defined,\n
		Height = user-defined,\n
		Circumferential Resolution = Height resolution (Number of slices) = user-defined (Slices of pie)\n
		Center of circular base = user-defined*/
		void update(HLdouble m_Radius, HLdouble m_Height, HLdouble m_ResolutionHeight, HLdouble m_ResolutionCircumference, HLdouble m_PositionX ,HLdouble m_PositionY,HLdouble m_PositionZ);

		/*! \brief This function is used to search for a Cone by a name and return a pointer to the Cone Object if found.
	
		The name is assigned by the QHRenderer::setName function. If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Cone* searchCone(char* ConeName);
		/*! \brief This function is used to search for a cone object by a ShapeID. The function returns a pointer to the matching Cone object.
	
		A shapeID is assigned to every haptic object by OpenHaptics. This is a unique number identifying
		the object under a fixed rendering context. This function can be used in events like "touch" or "untouch" etc... 
		which can be setup to invoke the callback functions and pass the ShapeID.
		The search function with the ShapeID can then be used to return a pointer to the primitive shape.
		If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Cone* searchCone(HLuint ConeID);

		///Destructor for the Cone class
		~Cone(void);

        ///This function returns the radius of the cylinder
        HLdouble getRadius(void);
        ///This function returns the height of the cylinder
        HLdouble getHeight(void);
};

#endif