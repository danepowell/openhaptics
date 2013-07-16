//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: Text.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the Text Class primitive with all associated 
// parameters. This class required "Test.ttf" default font to be present
// in the compile directory and uses the TrueType 2 font library.
// 
//
//---------------------------------------------------------------------

#ifndef TEXT_H
#define TEXT_H


#include "ExportDef.h"
#include "Shape.h"



typedef struct FT_FaceRec_*  FT_Face;



/*!\brief This class is used to display a Text string on screen

	Use this class to Display a Text string. 
		
	1) The class generates a bitmapped rendering of the text string provided.

	2) The class constructor contains provisions to select the font file (.ttf) to be used

	3) The size of the text is also adjustable

	Note 1: The text is displayed in Normalized Orthographic Coordinates. So (0,0) corresponds to the bottom left corner of the screen
	and (1.0,1.0) corresponds to the upper right corner.

	Note 2: The text is ALWAYS haptically invisible. (the Shape::setHapticVisibility function does not affect the text)

	In the setTranslation function the 'z' coordinate is user settable between 1 and -1. This corresponds to the near clipping plane and the 
	far clipping plane respectively. The constructor will by default set the text to be located with a z coordinate of -1. This is done so the 
	text does not occlude the cursor. The programmer can vary this setting to move the text forward or back if so desired.

*/

class QHAPI Text:public Shape
{
	
	GLuint * m_textures;	///< Holds the texture id's 
	GLuint m_list_base;	///< Holds the first display list id
	//The init function will create a font of
	//of the height h from the file fname.
	void init(const char * fname, unsigned int h);
	
	
	void print(char* Text);
	///Free all the resources assosiated with the font.
	void clean();

    ///This variable holds the text string
    char* m_pTextString;
	char* m_pFontFile;
    unsigned int m_FontSize;

	void make_dlist ( FT_Face face, char ch, GLuint m_list_base, GLuint * tex_base );
			///This function gets the first power of 2 >= the
	///int that we pass it.
	static int next_p2 ( int a );



	public:


		

	
	/*! \brief Default Text Constructor, Prints "hello World" at origin

	Font Size  = 32\n
	Position (Bottom Left of first letter) = (0.0, 0.0, 0.0)\n
	Font Style: Arial Black\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	Text(void);
	/*! \brief Text constructor with additional parameter, Font size and the text string.

	This update function updates the text string to a user defined string\n
	Font Size  = user-defined\n
	Position (Bottom Left of first letter) = (0.0, 0.0, 0.0)\n
	Font Style: Arial Black\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	Text(HLuint FontSize, char* Text);
	/*! \brief Text constructor with additional parameters, Font size, text string and Text File.

	This update function updates the text string to a user defined string\n
	Font Size  = user-defined\n
	Position (Bottom Left of first letter) = (0.0, 0.0, 0.0)\n
	Font File: user-defined\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	Text(HLuint FontSize, char* Text, char* FontFile);
	/*! \brief Text constructor with the position of the text also specified

	This update function updates the text string to a user defined string\n
	Font Size  = user-defined\n
	Position (Bottom Left of first letter) = user-defined\n
	Font Style: Arial Black\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	Text(HLuint FontSize, char* Text, HLdouble m_PositionX, HLdouble m_PositionY);
	/*! \brief Text constructior with position and rotation

	This update function updates the text string to a user defined string\n
	Font Size  = user-defined\n
	Position (Bottom Left of first letter) = user-defined\n
	Font File: user specified text (.ttf) file\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	Text(HLuint FontSize, char* Text, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble AxisX, HLdouble AxisY, HLdouble AxisZ, HLdouble m_RotationFlag, char* FontFile);


	/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

	The update function is used to update the Text to a default "Hello World". The user can change the Text or move it to a different position and orientation.
	This function is useful in callbacks where the user wants to change text based on an event. Say like changing a "failed" message to "success"
	based on some conditions.\n

	This update function updates the text string to "Hello World"\n
	Font Size  = 32\n
	Position (Bottom Left of first letter) = (0.0, 0.0, 0.0)\n
	Font Style: Arial Black\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	void update(void);
	/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

	The update function is used to update the Text. 
	This function is useful in callbacks where the user wants to change text based on an event. Say like changing a "failed" message to "success"
	based on some conditions.\n
	
	This update function updates the text string to a user defined string\n
	Font Size  = 32\n
	Position (Bottom Left of first letter) = (0.0, 0.0, 0.0)\n
	Font Style: Arial Black\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	void update(char* Text);
	/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

	The update function is used to update the Text. The user can change the Text or move it to a different position and orientation.
	This function is useful in callbacks where the user wants to change text based on an event. Say like changing a "failed" message to "success"
	based on some conditions.
	
	This update function updates the text string to a user defined string\n
	Font Size  = 32\n
	Position (Bottom Left of first letter) = user-defined\n
	Font Style: Arial Black\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane
	*/
	void update(char* Text, HLdouble m_PositionX, HLdouble m_PositionY);
	/*! \brief The update function act just like constructors, except that they can be invoked anywhere in the program

	The update function is used to update the Text. The user can change the Text or move it to a different position and orientation.
	This function is useful in callbacks where the user wants to change text based on an event. Say like changing a "failed" message to "success"
	based on some conditions.
	
	This update function updates the text string to a user defined string\n
	Font Size  = 32\n
	Position (Bottom Left of first letter) = user-defined\n
	Font File: user specified text (.ttf) file\n
	Text Cordinate system: Orthographics Projection, Bottom Left Corner is (0.0,0.0), Top Right Corner is (1.0,1.0)
	By default the 'z' coordinate is at -1. The z coordinate is adujustable between -1 and 1 (use setTranslation function), which corresponds to the far
	clipping plane and the near clipping plane. \n
	Note: The text can be rotated about any axis, however considering the 2D nature of text, for the best results use rotation about the 'z' axis.
	*/
	void update(char* Text, HLdouble m_PositionX, HLdouble m_PositionY, HLdouble AxisX, HLdouble AxisY, HLdouble AxisZ, HLdouble m_RotationFlag, char* FontFile);
	
	//Text Destructor
	~Text(void);



	/*! \brief This function is used to search for a text string by a name and return a pointer to the Text Object if found.
	
	The name is assigned by the QHRenderer::setName function. If the function does not find a match it will return a NULL pointer.
	It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
	static Text* searchText(char* TextName);
	/*! \brief This function is used to search for a text object by a ShapeID and return a pointer if found.
	
	A shapeID is assigned to every haptic object by OpenHaptics. This is a unique number identifying
	the object under a fixed rendering context. This function can be used in events like "touch" or "untouch" etc...
	which can be setup to invoke the callback functions and pass the ShapeID.
	The search function with the ShapeID can then be used to return a pointer to the primitive shape.
	If the function does not find a match it will return a NULL pointer.
	It is the responsibility of the user to detect the NULL return and handle it in a safe manner*/
	static Text* searchText(HLuint TextID);

    /*! \brief Function to change the rendered text at runtime.

	This function can be used in lieu of the update function. */
    void setText(char* Text);
    /*! \brief This function returns the string being rendered*/
	void getString(char* TextString);
    /*! \brief This function returns the current font size*/
    unsigned int getFontSize(void);
};


#endif
