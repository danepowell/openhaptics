/*****************************************************************************

Copyright (c) 2008 SensAble Technologies, Inc. All rights reserved.

QuickHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    ExportDef.h
    
Description: 

    Standard DLL export header.

******************************************************************************/
#ifndef EXPORTDEF_H
#define EXPORTDEF_H


#ifndef QH_EXPORT_H_DEFINE
#define QH_EXPORT_H_DEFINE

#if defined(WIN32)
# ifdef QH_EXPORTS
#  define QHAPI __declspec(dllexport)
# else
#  define QHAPI __declspec(dllimport)
# endif
# define QHAPIENTRY  __stdcall
# define QHCALLBACK  __stdcall
#endif // WIN32

#if defined(linux)
# define QHAPI
# define QHAPIENTRY  __attribute__((stdcall))
# define QHCALLBACK  __attribute__((stdcall))
#endif // linux

#if defined(__APPLE__)
# define QHAPI
# define QHAPIENTRY
# define QHCALLBACK
#endif // __APPLE__

#endif /* QH_EXPORT_H_DEFINE */

#endif

/******************************************************************************/
