#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SKY_DEF_H
#define _SKY_DEF_H

#ifndef PVOID
typedef void * PVOID;
#endif

#ifndef UCHAR
typedef unsigned char UCHAR, * PUCHAR;
#endif

#ifndef UINT
typedef unsigned int  UINT, * PUINT;
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define CRNL	"\r\n"

#endif // _SKY_DEF_H
#ifdef __cplusplus
}
#endif
