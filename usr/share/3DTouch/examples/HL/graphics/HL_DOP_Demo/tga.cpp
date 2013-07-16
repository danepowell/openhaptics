/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  tga.cpp

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

#if defined(WIN32)
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#include "tga.h"

// this variable is used for image series
static int savedImages=0;
TextureImage BGtextures[1];									// Storage For One Texture
// load the image header fields. We only keep those that matter!
void tgaLoadHeader(FILE *file, tgaInfo *info) {

	unsigned char cGarbage;
	short int iGarbage;

	fread(&cGarbage, sizeof(unsigned char), 1, file);
	fread(&cGarbage, sizeof(unsigned char), 1, file);

// type must be 2 or 3
	fread(&info->type, sizeof(unsigned char), 1, file);

	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&cGarbage, sizeof(unsigned char), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);

	fread(&info->width, sizeof(short int), 1, file);
	fread(&info->height, sizeof(short int), 1, file);
	fread(&info->pixelDepth, sizeof(unsigned char), 1, file);

	fread(&cGarbage, sizeof(unsigned char), 1, file);
}

// loads the image pixels. You shouldn't call this function
// directly
void tgaLoadImageData(FILE *file, tgaInfo *info) {

	int mode,total,i;
	unsigned char aux;

// mode equal the number of components for each pixel
	mode = info->pixelDepth / 8;
// total is the number of bytes we'll have to read
	total = info->height * info->width * mode;
	
	fread(info->imageData,sizeof(unsigned char),total,file);

// mode=3 or 4 implies that the image is RGB(A). However TGA
// stores it as BGR(A) so we'll have to swap R and B.
	if (mode >= 3)
		for (i=0; i < total; i+= mode) {
			aux = info->imageData[i];
			info->imageData[i] = info->imageData[i+2];
			info->imageData[i+2] = aux;
		}
}	

// this is the function to call when we want to load
// an image
tgaInfo * tgaLoad(char *filename) {
	
	FILE *file;
	tgaInfo *info;
	int mode,total;

// allocate memory for the info struct and check!
	info = (tgaInfo *)malloc(sizeof(tgaInfo));
	if (info == NULL)
		return(NULL);


// open the file for reading (binary mode)
	file = fopen(filename, "rb");
	if (file == NULL) {
		info->status = TGA_ERROR_FILE_OPEN;
		return(info);
	}

// load the header
	tgaLoadHeader(file,info);

// check for errors when loading the header
	if (ferror(file)) {
		info->status = TGA_ERROR_READING_FILE;
		fclose(file);
		return(info);
	}

// check if the image is color indexed
	if (info->type == 1) {
		info->status = TGA_ERROR_INDEXED_COLOR;
		fclose(file);
		return(info);
	}
// check for other types (compressed images)
	if ((info->type != 2) && (info->type !=3)) {
		info->status = TGA_ERROR_COMPRESSED_FILE;
		fclose(file);
		return(info);
	}

// mode equals the number of image components
	mode = info->pixelDepth / 8;

    // total is the number of bytes to read
	total = info->height * info->width * mode;
// allocate memory for image pixels
	info->imageData = (unsigned char *)malloc(sizeof(unsigned char) * 
															total);

// check to make sure we have the memory required
	if (info->imageData == NULL) {
		info->status = TGA_ERROR_MEMORY;
		fclose(file);
		return(info);
	}
// finally load the image pixels
	tgaLoadImageData(file,info);

    buildTGATexture(&BGtextures[0], filename); // Build the texture from tga. 
// check for errors when reading the pixels
	if (ferror(file)) {
		info->status = TGA_ERROR_READING_FILE;
		fclose(file);
		return(info);
	}
	fclose(file);
	info->status = TGA_OK;
	return(info);
}		

// converts RGB to greyscale
void tgaRGBtoGreyscale(tgaInfo *info) {

	int mode,i,j;

	unsigned char *newImageData;

// if the image is already greyscale do nothing
	if (info->pixelDepth == 8)
		return;

// compute the number of actual components
	mode = info->pixelDepth / 8;

// allocate an array for the new image data
	newImageData = (unsigned char *)malloc(sizeof(unsigned char) * 
											info->height * info->width);
	if (newImageData == NULL) {
		return;
	}

// convert pixels: greyscale = o.30 * R + 0.59 * G + 0.11 * B
	for (i = 0,j = 0; j < info->width * info->height; i +=mode, j++)
		newImageData[j] =	(unsigned char)(0.11 * info->imageData[i] + 
						0.59 * info->imageData[i+1] +
						0.3 * info->imageData[i+2]);


//free old image data
	free(info->imageData);

// reassign pixelDepth and type according to the new image type
	info->pixelDepth = 8;
	info->type = 3;
// reassing imageData to the new array.
	info->imageData = newImageData;
}

// takes a screen shot and saves it to a TGA image
int tgaGrabScreenSeries(char *filename, int x,int y, int w, int h) {
	
	unsigned char *imageData;

// allocate memory for the pixels
	imageData = (unsigned char *)malloc(sizeof(unsigned char) * w * h * 4);

// read the pixels from the frame buffer
	glReadPixels(x,y,w,h,GL_RGBA,GL_UNSIGNED_BYTE, (GLvoid *)imageData);

// save the image 
	return(tgaSaveSeries(filename,w,h,32,imageData));
}

// saves an array of pixels as a TGA image
int tgaSave(char			*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData) {

	unsigned char cGarbage = 0, type,mode,aux;
	short int iGarbage = 0;
	int i;
	FILE *file;

// open file and check for errors
	file = fopen(filename, "wb");
	if (file == NULL) {
		return(TGA_ERROR_FILE_OPEN);
	}

// compute image type: 2 for RGB(A), 3 for greyscale
	mode = pixelDepth / 8;
	if ((pixelDepth == 24) || (pixelDepth == 32))
		type = 2;
	else
		type = 3;

// write the header
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	fwrite(&type, sizeof(unsigned char), 1, file);

	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);

	fwrite(&width, sizeof(short int), 1, file);
	fwrite(&height, sizeof(short int), 1, file);
	fwrite(&pixelDepth, sizeof(unsigned char), 1, file);

	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

// convert the image data from RGB(a) to BGR(A)
	if (mode >= 3)
	for (i=0; i < width * height * mode ; i+= mode) {
		aux = imageData[i];
		imageData[i] = imageData[i+2];
		imageData[i+2] = aux;
	}

// save the image data
	fwrite(imageData, sizeof(unsigned char), width * height * mode, file);
	fclose(file);
// release the memory
	free(imageData);

	return(TGA_OK);
}

// saves a series of files with names "filenameX.tga"
int tgaSaveSeries(char		*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData) {
	
	char *newFilename;
	int status;
// compute the new filename by adding the series number and the extension

	newFilename = (char *)malloc(sizeof(char) * strlen(filename)+8);

	sprintf(newFilename,"%s%d.tga",filename,savedImages);
// save the image
	status = tgaSave(newFilename,width,height,pixelDepth,imageData);
//increase the counter
	savedImages++;
	return(status);
}

bool buildTGATexture(TextureImage *texture, char *filename)			// Loads A TGA File Into Memory
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0 ,0,0};	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;										// Temporary Variable
	GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)

	FILE *file = fopen(filename, "rb");						// Open The TGA File

	if(	file==NULL ||										// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL)									// Did The File Even Exist? *Added Jim Strong*
			return false;									// Return False
		else
		{
			fclose(file);									// If Anything Failed, Close The File
			return false;									// Return False
		}
	}

	texture->width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texture->width	<=0	||								// Is The Width Less Than Or Equal To Zero
		texture->height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

	texture->bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture->width*texture->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture->imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if(	texture->imageData==NULL ||							// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture->imageData!=NULL)						// Was Image Data Loaded
			free(texture->imageData);						// If So, Release The Image Data

		fclose(file);										// Close The File
		return false;										// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)		// Loop Through The Image Data
	{														// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texture->imageData[i];							// Temporarily Store The Value At Image Data 'i'
		texture->imageData[i] = texture->imageData[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
		texture->imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);											// Close The File

	// Build A Texture From The Data
	glGenTextures(1, &texture[0].texID);					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture[0].texID);			// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	
	if (texture[0].bpp==24)									// Was The TGA 24 Bits
	{
		type=GL_RGB;										// If So Set The 'type' To GL_RGB
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture[0].imageData);

	return true;											// Texture Building Went Ok, Return True
}

// releases the memory used for the image
void tgaDestroy(tgaInfo *info) {

	if (info != NULL) {
		free(info->imageData);
		free(info);
	}
}
