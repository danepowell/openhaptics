//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Line.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the Line Class primitive with all associated 
// parameters.
// 
//
//---------------------------------------------------------------------

#ifndef LINE_H
#define LINE_H


#include "ExportDef.h"
#include "Shape.h"


/*!\brief This class loads a Line shape.

	Use this class to load a Line. The class generates a display list for the Line and then
	supplies display list to the parent shape class.
*/

class QHAPI Line: public Shape
{
	////////////////////
	////////////////////
		///Line width
		GLfloat m_LineWidth; 
		///m_Start end end points of the line
		hduVector3Dd m_Start,m_Stop;
		///Function to generate an OpenGL display List for a line
		virtual void PureGLLine(hduVector3Dd m_Start, hduVector3Dd m_Stop,HLfloat m_Width);

	public:

		/*! \brief Default Constructor

		The default Line parameters are:\n
		Start Point = (0.0, 0.0, 0.0)\n
		End Point = (1.0, 1.0, 1.0)\n
		Line Width = 1.0;*/
		Line(void);
		/*! \brief Line constructor with start and end points

		The parameters to this contructor are:\n
		Start Point = user-defined\n
		End Point = user-defined\n
		Line Width = 1.0;*/
		Line(HLdouble x1,HLdouble y1,HLdouble z1, HLdouble x2, HLdouble y2, HLdouble z2);
		/*! \brief Line constructor with Start, End points and Line Width

		The parameters to this contructor are:\n
		Start Point = user-defined\n
		End Point = user-defined\n
		Line Width = user-defined;*/
		Line(HLdouble x1,HLdouble y1,HLdouble z1, HLdouble x2, HLdouble y2, HLdouble z2, HLfloat m_Width);

		/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program. 

		The update function is used to update the Line. The default values for update are:\n

		Start Point = (0.0, 0.0, 0.0)\n
		End Point = (1.0, 1.0, 1.0)\n
		Line Width = 1.0;*/
		void update(void);
		/*! \brief The update functions act just like constructors, except that they can be invoked anywhere in the program. 

		The update function is used to update the Line. This function inputs:\n
		Start Point = user-defined\n
		End Point = user-defined\n
		Line Width = 1.0;*/
		void update(HLdouble x1,HLdouble y1,HLdouble z1, HLdouble x2, HLdouble y2, HLdouble z2);
		/*! \brief The update functions act just like constructors, except that they can be invoked anywhere in the program. 

		The update function is used to update the Line. This function inputs:\n
		Start Point = user-defined\n
		End Point = user-defined\n
		Line Width = user-defined;*/
		void update(HLdouble x1,HLdouble y1,HLdouble z1, HLdouble x2, HLdouble y2, HLdouble z2, HLfloat m_Width);

		/*! \brief This function is used to search for a Line by a name and return a pointer to the Line Object if found.
	
		The name is assigned by the QHRenderer::setName function. If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Line* searchLine(char* LineName);
		/*! \brief This function is used to search for a Line object by a ShapeID and return a pointer to the Line if found.
	
		A shapeID is assigned to every haptic object by OpenHaptics. This is a unique number identifying
		the object under a fixed rendering context. This function can be used in events like "touch" or "untouch" etc... 
		which can be setup to invoke the callback functions and pass the ShapeID.
		The search function with the ShapeID can then be used to return a pointer to the primitive shape.
		If the function does not find a match it will return a NULL pointer.
		It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
		static Line* searchLine(HLuint LineID);
		
		
		///Explicitly set the width of the line
		void setWidth(GLfloat m_LineWidth);
        ///Get the start point of the line
        hduVector3Dd getStartPoint(void);
        ///Get the end point of the line
        hduVector3Dd getEndPoint(void);
        ///Get the width of the line
        GLfloat getLineWidth(void);

};

#endif