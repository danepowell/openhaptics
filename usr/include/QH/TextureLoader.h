//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: TextureLoader.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: This class loads the texture JPGs using the DevIL libraries
// I have almost entirely rewritten Chris Leathley texture loader. 
// The primary texture loader now is DEVIL, I retained the class structure
// because at this point the uAPI is almost complete and I want to keep the 
// existing class structure.
//---------------------------------------------------------------------------





/////////////////////////////////////////////////////////////////////////////
//                                                                          /
//m_Texture Loader                                                            /
//                                                                          /
//Originally Based on Jeff Molofee's IPicture Basecode                      /
//Extensions By Chris Leathley (http://members.iinet.net.au/~cleathley/)    /
//                                                                          /
/////////////////////////////////////////////////////////////////////////////
//                                                                          /
// Loads  : BMP, EMF, GIF, ICO, JPG, WMF and TGA                            /
// Source : Reads From Disk, Ram, Project Resource or the Internet          /
// Extras : Images Can Be Any m_Width Or m_Height                               /
//          Low Quality Textures can be created                             /
//          Different Filter Level Support (None, Bilinear and Trilinear    /
//          Mipmapping Support                                              /
//                                                                          /
/////////////////////////////////////////////////////////////////////////////
//                                                                          /
//  Free To Use In Projects Of Your Own.  All I Ask For Is m_A Simple Greet   /
//  Or Mention of my site in your readme or the project itself :)           /

#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H


#pragma warning(disable:4996)//Get rid of depricated warinigs


#include "ExportDef.h"
#include "Globals.h"






// Define Interface Data Types / Structures
typedef	struct
{
	GLuint		m_pTextureID;									// m_Texture ID Used To Select m_A m_Texture
} glTexture;


// define TextureLoader Class
//
class QHAPI TextureLoader
{
	public:
							//Constructor to initialise IL ILU and ILUT texture
							TextureLoader();
							//Destructor to free memory
							virtual				~TextureLoader();
	
							//This function loads an image file from disk and convertes it into an OpenGl texture ID
		int					loadTextureFromDisk(char *szFileName, glTexture *pglTexture);
							//Function to clean up m_textures.
	

private:
	friend class Shape;	
	void				FreeTexture(glTexture *pglTexture);
	// variables
	
};

#endif



