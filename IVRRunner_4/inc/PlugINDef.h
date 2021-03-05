#ifndef __PLUG_IN_DEF__H_
#define __PLUG_IN_DEF__H_
#include "IVRAuth.h"
#include "IVRCon.h"
#include "Connection.h"

typedef struct tag_HANDLE_BUFFER
{
	char  szModName[256];
	HMODULE hMod;
} HANDLE_BUFFER;

typedef BOOL (InitPlugin)(int, char *);
typedef BOOL (ReleasePlugin)(int );
typedef int (VirtualFunc)(IVR_AUTHENTICATION_PACKET *, int * , char [20][128],int * , int* , int* );
// sever address, server port, authentication packet
// ment buffer, ment count, retrial count
typedef int (DongSeoBilling)(IVR_AUTHENTICATION_PACKET *, char [20][128], int *, int *);
typedef int (LocalDBCon)(IVR_AUTHENTICATION_PACKET *, CALLINFOMATION *, int *);
typedef int (GetExValue)(int , int , char **);
typedef void (ResetValue)(int );
typedef int (DongSeoGK)(IVR_AUTHENTICATION_PACKET *, char [20][128], int *, int *, int *, int *);
#endif // __PLUG_IN_DEF__H_