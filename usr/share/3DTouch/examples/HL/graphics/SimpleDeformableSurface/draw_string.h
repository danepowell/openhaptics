/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  draw_string.h

Description:

  Simple function for drawing strings using glutBitmapCharacter.
*******************************************************************************/

#if !defined(_DRAW_STRING_H_)
#define _DRAW_STRING_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

void DrawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...);

#endif // !defined(_DRAW_STRING_H_)
