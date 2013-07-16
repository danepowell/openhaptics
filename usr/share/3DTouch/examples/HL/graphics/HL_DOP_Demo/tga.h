/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  tga.h

Description: 

This is a very simple TGA lib. It will only load and save 
uncompressed images in greyscale, RGB or RGBA mode.
Original source from OpenGL Lighthouse3D tutorials. 
http://www.lighthouse3d.com/opengl/terrain/
Has been modified to accomodate to the current project by 
adding functions to do OpenGL texture mapping and modify 
intensity values. 

Author: Venkat Gourishankar
******************************************************************************/

#ifndef _TGA_H_
#define _TGA_H_

#define	TGA_ERROR_FILE_OPEN				-5
#define TGA_ERROR_READING_FILE			-4
#define TGA_ERROR_INDEXED_COLOR			-3
#define TGA_ERROR_MEMORY				-2
#define TGA_ERROR_COMPRESSED_FILE		-1
#define TGA_OK							 0


typedef struct {
    GLuint		texID;									// Texture ID Used To Select A Texture
	int status;
	unsigned char type, pixelDepth;
	short int width, height;
	unsigned char *imageData;
//    GLubyte *imageData;
}tgaInfo;

typedef struct												// Create A Structure
{
	GLubyte	*imageData;										// Image Data (Up To 32 Bits)
	GLuint	bpp;											// Image Color Depth In Bits Per Pixel.
	GLuint	width;											// Image Width
	GLuint	height;											// Image Height
	GLuint	texID;											// Texture ID Used To Select A Texture
} TextureImage;												// Structure Name

tgaInfo* tgaLoad(char *filename);

int tgaSave(char			*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth, 
			 unsigned char	*imageData);

int tgaSaveSeries(char			*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth, 
			 unsigned char	*imageData);

void tgaRGBtoGreyscale(tgaInfo *info);

int tgaGrabScreenSeries(char *filename, int x,int y, int w, int h);

bool buildTGATexture(TextureImage *texture, char *filename);

void tgaDestroy(tgaInfo *info);


#endif