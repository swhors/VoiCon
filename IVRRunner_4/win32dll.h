
/////////////////////////////////////////////////////////////////////////
/* 
	Copyright (C) SafeNet China Ltd.	
	All rights reserved.

  File name:	win32dll.h
  File description:	
	the head file of API for SoftDog 

	Date :	2003-5-26
	Author:	Jason Jiang



	Change history:		
	----Version:----Mender Name:----Date:----Change description----		
		3.1			Jason Jiang		2003-5-26 Create the file


*/ 
/////////////////////////////////////////////////////////////////////////

/******************************************************************************\
*                  FUNCTION PROTOTYPES IN Win32DLL.DLL
\******************************************************************************/
#ifndef _WIN32DLL_H
#define _WIN32DLL_H

#include "stdafx.h"

// Read data from the dog
typedef DWORD (WINAPI * DOGREAD) (DWORD dwBytes, DWORD dwAddr, char *pData);

DOGREAD DogRead = NULL;

// Write data to the dog
typedef DWORD (WINAPI * DOGWRITE) (DWORD dwBytes, DWORD dwAddr, char *pData);

DOGWRITE DogWrite = NULL;

#endif	// Win32Dll_h