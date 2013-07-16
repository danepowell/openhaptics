//-----------------------------------------------------------------
//
// Copyright 1998, SensAble Technologies, Inc.
//
// File: CompilerConfig.h
//
// Author: Brad Payne, Modified: Hari Vasudevan
//
// Created: 9/15/98
// Modified: 09/11/2008
//
//-----------------------------------------------------------------

#ifndef CXX_FS_CompilerConfig_DEFINE
#define CXX_FS_CompilerConfig_DEFINE

#ifdef WIN32

//When the STL container is empty, accessing element 0 is an "out of range" error
//if the following macro is defined as 1 (the default).

#ifdef _SECURE_SCL
# undef _SECURE_SCL
#endif
#define _SECURE_SCL 0

//TURN OFF INTERATOR DEBUGGING FOR PERFORMANCE
#ifdef _HAS_ITERATOR_DEBUGGING
# undef _HAS_ITERATOR_DEBUGGING
#endif
#define _HAS_ITERATOR_DEBUGGING 0

//Disable CRT warning: "This function or variable may be unsafe"
#ifndef _CRT_SECURE_NO_DEPRECATE
# define _CRT_SECURE_NO_DEPRECATE
#endif

//Disable CRT warning: "The POSIX name for this item is deprecated."
#ifndef _CRT_NONSTDC_NO_DEPRECATE
# define _CRT_NONSTDC_NO_DEPRECATE
#endif

// truncating variables, e.g. double to float
#pragma warning( disable: 4244 )
// truncation from const double to float
#pragma warning( disable: 4305 )
// STL relops colliding with other comparison operators
#pragma warning( disable: 4666 )
// "ID truncated to 255 characters for debugging."
#pragma warning( disable: 4786 ) 
// "forcing value to bool 'true' or 'false' (performance warning)"
#pragma warning( disable: 4800 ) 
// dominance with virtual base classses
#pragma warning( disable: 4250 ) 
// class 'type' needs to have dll-interface
#pragma warning( disable: 4251 )
// non dll-interface class foo used as base for dll-interface class bar
#pragma warning( disable: 4275 )
// When compiling with /Wp64, or when compiling on a 64-bit operating system, 
// type is 32 bits but size_t is 64 bits when compiling for 64-bit targets. 
#pragma warning( disable: 4267 )
// nonstandard extension used: enum 'enum' used in qualified name
// When you refer to an enum inside a type, you do not need to specify the name of the enum.
#pragma warning( disable: 4482 )
// signed/unsigned mismatch
#pragma warning( disable: 4018 )

// function redefinition warning ignore. This is because of the DevIL library having multiply linked libraries
#pragma warning( disable: 4006 )




#endif // WIN32

#endif // CXX_FS_CompilerConfig_DEFINE