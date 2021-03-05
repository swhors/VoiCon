/**
@file

@brief Type definitions and helper macros which aren't part of Standard C++

For latest source code see http://www.tixy.clara.net/source/

Copyright (C) 2004	J.D.Medhurst (a.k.a. Tixy)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// Following comment is for Doxygen to generate the Main Page...

/**
@mainpage
<H1>Tixy's Source Code</H1>

C++ and Forth source code.

The Forth code (and the VM implementation in C++) are Public Domain;
they may be used, modified and distibuted in any manner, without any restriction.
Other code is provided under the terms of the <A HREF="../gpl.txt">GNU General Public License</A>.
(See <A HREF="http://www.gnu.org/licenses/licenses.html">GNU Licenses</A> for more information.)

The code has only been tested on 32bit little-endian targets but has been designed to be
word size and endian agnostic; please let me know if it doesn't work on your system.

Source code is commented using <A HREF="http://www.doxygen.org">Doxygen</A> style comments
and is formatted with tab stops every four characters.

For latest source code see http://www.tixy.clara.net/source/

Copyright (C) 2004-2005 J.D.Medhurst (a.k.a. Tixy)
*/


#ifndef __COMMON_H__
#define __COMMON_H__


#if 0
#define DEBUG	/**< Defined when compiling code for debugging */
#endif

/**
@defgroup integers Common - Basic Integer Types.

These definitions will need to be modified on systems where 'char', 'short' and 'int'
have sizes different from 8, 16 and 32 bits.

Note, 'int' is assumed to be in 2s complement format and at least 32 bits in size.
@{
*/
typedef unsigned char  uint8;	/**< An 8 bit unsigned integer */
typedef unsigned short uint16;	/**< An 16 bit unsigned integer */
typedef unsigned int   uint32;	/**< An 32 bit unsigned integer */
typedef signed char    int8;	/**< An 8 bit signed integer (2s complement) */
typedef signed short   int16;	/**< An 16 bit signed integer (2s complement) */
typedef signed int	   int32;	/**< An 32 bit signed integer (2s complement) */
typedef unsigned int   uint;	/**< An unsigned integer of at least 32 bits */
/** @} */ // End of group


#if defined(__EPOC32__) || defined(__WINS__) || defined(__WINSCW__)
// Compiling for Symbian OS...

#include <e32std.h>

#if !defined(__BREAKPOINT)
#if defined(__WINS__)
#define BREAKPOINT { _asm int 3 }			/**< Invoke debugger */
#elif defined(__WINSCW__)
#define BREAKPOINT { _asm byte 0xcc }		/**< Invoke debugger */
#else
#define BREAKPOINT							/**< Invoke debugger */
#endif
#else
#define BREAKPOINT	{__BREAKPOINT()}		/**< Invoke debugger */
#endif

#define IMPORT		IMPORT_C				/**< Mark a function which is to be imported from a DLL */
#define EXPORT		EXPORT_C				/**< Mark a function to be exported from a DLL */

#undef ASSERT
#define ASSERT(c)	{if(!(c)) BREAKPOINT;}	/**< Assert that expression 'c' is true */

#if !defined(DEBUG) && defined(_DEBUG)
#define DEBUG								/**< Defined when compiling code for debugging */
#endif


#elif defined(_MSC_VER)
// Compiling for Microsoft Visual C++...

#ifndef BREAKPOINT
#define BREAKPOINT	{ _asm int 3 }			/**< Invoke debugger */
#endif

#ifndef IMPORT
#define IMPORT		__declspec(dllexport)	/**< Mark a function which is to be imported from a DLL */
#endif

#ifndef EXPORT
#define EXPORT		__declspec(dllexport)	/**< Mark a function to be exported from a DLL */
#endif

#ifndef ASSERT
#define ASSERT(c)	{if(!(c)) BREAKPOINT;}	/**< Assert that expression 'c' is true */
#endif


#else
// Unknown system...
#define BREAKPOINT							/**< Invoke debugger */
#define IMPORT								/**< Mark a function which is to be imported from a DLL */
#define EXPORT								/**< Mark a function to be exported from a DLL */
#define ASSERT(c)							/**< Assert that expression 'c' is true */


#endif


#if defined(_MSC_VER) // disable anoying warnigs from MSVC...
#pragma warning( disable : 4244 )	/* convertion from x to y, possible loss of data */
#pragma warning( disable : 4514 )	/* unreferenced inline function has been removed */
#pragma warning( disable : 4146 )	/* unary minus operator applied to unsigned type, result still unsigned */
#pragma warning( disable : 4710 )	/* function x not inlined */
#pragma warning( disable : 4355 )   /* 'this' : used in base member initializer list */
#endif


#ifdef DEBUG
#define ASSERT_DEBUG(c) ASSERT(c)	/**< Assert that expression 'c' is true (when compiled for debugging)*/
#else
#define ASSERT_DEBUG(c) 			/**< Assert that expression 'c' is true (when compiled for debugging)*/
#endif

#ifndef ASSERT_COMPILE
/** Assert, at compile time, that expression 'c' is true. */
#define ASSERT_COMPILE(c) void assert_compile(int assert_compile[(c)?1:-1])
#endif

#ifndef NULL
#define NULL 0		/**< Used to represent a null pointer type */
#endif


#endif
