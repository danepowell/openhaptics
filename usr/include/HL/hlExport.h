/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    hlExport.h
    
Description: 

    Standard DLL export header.

******************************************************************************/

#ifndef HL_EXPORT_H_DEFINE
#define HL_EXPORT_H_DEFINE

#if defined(WIN32)
# ifdef HL_EXPORTS
#  define HLAPI __declspec(dllexport)
# else
#  define HLAPI __declspec(dllimport)
# endif
# define HLAPIENTRY  __stdcall
# define HLCALLBACK  __stdcall
#endif // WIN32

#if defined(linux)
# define HLAPI
# define HLAPIENTRY  __attribute__((stdcall))
# define HLCALLBACK  __attribute__((stdcall))
#endif // linux

#if defined(__APPLE__)
# define HLAPI
# define HLAPIENTRY
# define HLCALLBACK
#endif // __APPLE__

#endif /* HL_EXPORT_H_DEFINE */

/******************************************************************************/
