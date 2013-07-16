/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    HdExport.h
    
Description: 

    Standard DLL export header.

******************************************************************************/

#ifndef HD_EXPORT_H_DEFINE
#define HD_EXPORT_H_DEFINE

#if defined(WIN32)
#  ifdef HD_EXPORTS
#     define HDAPI __declspec(dllexport)
#  else
#     define HDAPI __declspec(dllimport)
#  endif
#  define HDAPIENTRY  __stdcall
#  define HDCALLBACK  __stdcall
#endif /* WIN32 */

#if defined(linux)
#  define HDAPI
#  define HDAPIENTRY  __attribute__((stdcall))
#  define HDCALLBACK  __attribute__((stdcall))
#endif /* linux */

#if defined(__APPLE__)
#  define HDAPI
#  define HDAPIENTRY
#  define HDCALLBACK
#endif /* __APPLE__ */

#endif /* HD_EXPORT_H_DEFINE */

/******************************************************************************/
