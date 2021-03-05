#pragma once

#ifdef __cplusplus
extern "C"{
#endif

#ifndef _SKY_THREAD_H
#define _SKY_THREAD_H

#include "sky_def.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef LINUX
typedef void * SKYTHREAD;
typedef void * SKYINVAL;
typedef int    SKYRET;
typedef int    SKYHANDLE;
#define THREAD_RETURN NULL
#elif   defined  WIN32
//typedef DWORD  WINAPI SKYTHREAD;
typedef DWORD  SKYTHREAD;
typedef LPVOID SKYINVAL;
typedef HANDLE SKYRET;
typedef HANDLE SKYHANDLE;
#define THREAD_RETURN 0
#endif

SKYRET sky_thread_create(IN void *p_func,IN void *p_arg,IN int ext_opt, int memsize);

void sky_thread_close(SKYHANDLE id);



#endif // _SKY_THREAD_H

#ifdef __cplusplus
}
#endif
