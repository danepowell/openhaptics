//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Sphere.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the Sphere Class primitive with all associated 
// parameters.
// 
//
//---------------------------------------------------------------------

#ifndef SPHERE_H
#define SPHERE_H


#include "ExportDef.h"
#include "Shape.h"

/*!\brief This class to loads  a Sphere shape.

	Use this class to load a Sphere. The class generates a display list for the Sphere and then
	supplies display list to the parent shape class.
*/

class QHAPI Sphere: public Shape
{
	/////////////
	
	virtual void pureGLSphere(HLdouble m_Radius, HLdouble Resolution,hduVector3Dd m_Position);
	///Sphere parameters
	HLdouble m_Radius;
	///Sphere parameters
	HLdouble Resolution;
	///Sphere parameters
	HLdouble m_PositionX;
	///Sphere parameters
	HLdouble m_PositionY;
	///Sphere parameters
	HLdouble m_PositionZ;
	
	public:
		
	
		/*! \brief Default Constructor

		The parameters for this constructor are:\n
		Radius = 0.5\n
		Resolution = 20 (Number of slices)
		Position = (0.0, 0.0, 0.0)*/
		Sphere(void);
		/*! Constructor 

		The parameters for this constructor are:\n
		Radius = user-defined\n
		Resolution = user-defined (Number of slices)
		Position = (0.0, 0.0, 0.0)*/
		Sphere(HLdouble m_Radius, HLdouble Resolution);
		/*! Constructor 

		The parameters for this constructor are:\n
		Radius = user-defined\n
		Resolution = user-defined (Number of slices)
		Position = user-defined*/
		Sphere(HLdouble m_Radius, HLdouble Resolution, HLdouble m_PositionX ,HLdouble m_PositionY,HLdouble m_PositionZ);

		
		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

		The update function is used to update the Sphere. The parameters for this function are:\n
		Radius = 0.5\n
		Resolution = 20 (Number of slices)
		Position = (0.0, 0.0, 0.0)*/
		void update(void);
		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

		The update function is used to update the Sphere.The parameters for this function are:\n
		Radius = user-defined\n
		Resolution = user-defined (Number of slices)
		Position = (0.0, 0.0, 0.0)*/
		void update(HLdouble m_Radius, HLdouble Resolution);
		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

		The update function is used to update the Sphere. The parameters for this function are:\n
		Radius = user-defined\n
		Resolution = user-defined (Number of slices)
		Position = user-defined*/
		void update(HLdouble m_Radius, HLdouble Resolution, HLdouble m_PositionX ,HLdouble m_PositionY,HLdouble m_PositionZ);//m_Radius

		/*! \brief This function is used to search for a Sphere by a name and return a pointer to the Sphere Object if found.
	
		The name is assigned by the QHRenderer::setName function. If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Sphere* searchSphere(char* SphereName);
		/*! \brief This function is used to search for a Sphere object by a ShapeID and return a pointer to the Sphere if found.
	
		A shapeID is assigned to every haptic object by OpenHaptics. This is a unique number identifying
		the object under a fixed rendering context. This function can be used in events like "touch" or "untouch" etc...
		which can be setup to invoke the callback functions and pass the ShapeID.
		The search function with the ShapeID can then be used to return a pointer to the primitive shape.
		If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Sphere* searchSphere(HLuint SphereID);

        /*! \brief this function returns the radius of the Sphere*/
		HLdouble getRadius(void);

		~Sphere(void);
	////////
};


#endif
